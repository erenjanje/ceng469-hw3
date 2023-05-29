#include "things.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <array>
#include <glm/geometric.hpp>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <unordered_set>

#include "object.hpp"
#include "cubemap.hpp"
#include "utils.hpp"

extern const int width;
extern const int height;

static const auto quad_vertices = std::vector<float>{
    -1.0, -1.0,  0.0,
     1.0, -1.0,  0.0,
     1.0,  1.0,  0.0,
    -1.0,  1.0,  0.0,
};

static const auto quad_indices = std::vector<GLuint>{
    0, 1, 2,
    2, 3, 0,
};

static const auto quad_normals = std::vector<float>{};

static const auto cubemap_names = std::array<std::string, 6>{
    "cubemaps/px.png",
    "cubemaps/nx.png",
    "cubemaps/py.png",
    "cubemaps/ny.png",
    "cubemaps/pz.png",
    "cubemaps/nz.png",
};

static auto quad = Object();
static auto shader = ShaderProgram();
static auto camera = PerspectiveCamera();
static auto cubemap = Cubemap();

void init() {
    quad = Object(quad_vertices, quad_normals, quad_indices);
    quad.get_translation() = glm::vec3(0.0f, 0.0f, -1.0f);
    shader = ShaderProgram::from_files("vertex.glsl", "fragment.glsl");
    cubemap = Cubemap(cubemap_names);
}

void update() {
    handle_keys();
}

void draw() {
    shader.use();
    shader.set("camera_direction", camera.direction);
    cubemap.bind();
    quad.draw(shader, camera);
}

static std::unordered_set<int> pressed_keys;

#define ROTATION_ANGLE 0.06f

void handle_key(const int key) {
    const auto up_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto right_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(ROTATION_ANGLE), glm::vec3(1.0f, 0.0f, 0.0f));
    const auto down_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-ROTATION_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto left_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-ROTATION_ANGLE), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 current;
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};

    switch(key) {
        case GLFW_KEY_LEFT:
            current = up_rotation;
            rotation.y -= ROTATION_ANGLE;
        break;
        case GLFW_KEY_RIGHT:
            current = down_rotation;
            rotation.y += ROTATION_ANGLE;
        break;
        case GLFW_KEY_UP:
            current = right_rotation;
            rotation.x += ROTATION_ANGLE;
        break;
        case GLFW_KEY_DOWN:
            current = left_rotation;
            rotation.x -= ROTATION_ANGLE;
        break;
    }
    // camera.up = glm::normalize(glm::vec3(current * glm::vec4(camera.up, 0.0f)));
    // camera.direction = glm::normalize(glm::vec3(current * glm::vec4(camera.direction, 0.0f)));
    // quad.get_translation() = camera.direction;
    // quad.rotate(rotation);
    // LOG(quad.get_rotation());
    // LOG(quad.get_translation());
    // LOG(camera.direction);
}

void handle_keys() {
    for(const int& key : pressed_keys) {
        handle_key(key);
    }
}

void key_event(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE and action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if(key == GLFW_KEY_Q) {
        return;
    }
    if(action == GLFW_PRESS) {
        pressed_keys.insert(key);
    } else if(action == GLFW_RELEASE) {
        const auto found = pressed_keys.find(key);
        if(found != pressed_keys.end()) {
            pressed_keys.erase(pressed_keys.find(key));
        }
    }
}

Image::Image() {}

Image::Image(std::string filename) {
    data = stbi_load(filename.c_str(), &width, &height, &channel_number, 0);
    if(data == nullptr) {
        throw std::runtime_error("Loading Image \"" + filename + "\"");
    }
}

void Image::destroy() {
    stbi_image_free(data);
    width = 0;
    height = 0;
    data = nullptr;
}
