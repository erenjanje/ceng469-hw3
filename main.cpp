#include <memory>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <array>
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <chrono>

#include <signal.h>
#include <execinfo.h>

#include "things.hpp"

#define CHECK(expr, error_val) check_error<typeof(expr)>(expr, error_val, __FILE__, __LINE__, __FUNCTION__)

template<typename T>
T check_error(T value, const T& error_val, const char* filename, int line, const char* funcname) {
    if(value == error_val) {
        std::fprintf(stderr, "Error in\n   File: \"%s\"\n   Line: %d\n   Function: \"%s\"\n", filename, line, funcname);
        std::exit(EXIT_FAILURE);
    }
    return value;
}

static void glfw_error_cb(int error, const char* description) {
    std::fprintf(stderr, "GLFW Error %d:\n   %s\n", error, description);
}

static void segv_handler(int signo) {
    std::cout << signo << "\n";
    std::array<void*, 10> array;
    const auto size = backtrace(array.data(), array.size());
    const auto strings = backtrace_symbols(array.data(), size);
    if(strings != nullptr) {
        std::cout << "Caught segmentation fault" << "\n";
        for(int i = 0; i < size; i++) {
            std::cout << "   " << strings[i] << "\n";
        }
    }
    std::free(strings);
}

extern const int width = 800;
extern const int height = 800;
#include <algorithm>

int main() {
    signal(SIGSEGV, segv_handler);
    glfwSetErrorCallback(glfw_error_cb);
    CHECK(glfwInit(), GLFW_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    auto window = CHECK(glfwCreateWindow(width, height, "Window", nullptr, nullptr), nullptr);
    glfwSetKeyCallback(window, key_event);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    int w, h;
    auto prev = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    
    init();
    while(not glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        now = std::chrono::high_resolution_clock::now();
        const auto diff = std::chrono::duration<double>(now - prev);

        update(diff.count());
        draw();

        prev = std::chrono::high_resolution_clock::now();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
