#include "things.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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

static auto camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);

void init() {
    quad = Object(quad_vertices, quad_normals, quad_indices);
    quad.get_model() = glm::translate(glm::mat4(1.0f), camera.direction);
    shader = ShaderProgram::from_files("vertex.glsl", "fragment.glsl");
    cubemap = Cubemap(cubemap_names);
}

void update(double dt) {
    // LOG(1.0/dt);
    handle_keys();
}

void draw() {
    shader.use();
    shader.set("camera.position", camera_pos);
    shader.set("camera.direction", camera.direction);
    shader.set("camera.up", camera.up);
    cubemap.bind();
    quad.draw(shader, camera);
}

static std::unordered_set<int> pressed_keys;

#define ROTATION_ANGLE 0.02f
#define TRANSLATION 0.01f

// Why I don't have this function!!!!!
static glm::quat angleAxis(const float angle, const glm::vec3 axis) {
    return glm::normalize(glm::quat(
        std::cos(glm::radians(angle/2.0f)),
        std::sin(glm::radians(angle/2.0f)) * axis
    ));
}

void handle_key(const int key) {
    const auto left = key == GLFW_KEY_LEFT;
    const auto right = key == GLFW_KEY_RIGHT;
    const auto up = key == GLFW_KEY_UP;
    const auto down = key == GLFW_KEY_DOWN;
    const auto q = key == GLFW_KEY_Q;
    const auto e = key == GLFW_KEY_E;
    const auto right_vec = glm::cross(camera.direction, camera.up);
    const auto rotation_quat =
        glm::angleAxis(up*ROTATION_ANGLE - down*ROTATION_ANGLE, right_vec)
        * glm::angleAxis(left*ROTATION_ANGLE - right*ROTATION_ANGLE, camera.up)
        * glm::angleAxis(e*ROTATION_ANGLE - q*ROTATION_ANGLE, camera.direction);
    camera.direction = glm::vec3(rotation_quat * glm::vec4(camera.direction, 0.0f));
    camera.up = glm::vec3(rotation_quat * glm::vec4(camera.up, 0.0f));
    quad.get_model() = glm::toMat4(rotation_quat) * quad.get_model();

    const auto w = key == GLFW_KEY_W;
    const auto a = key == GLFW_KEY_A;
    const auto s = key == GLFW_KEY_S;
    const auto d = key == GLFW_KEY_D;
    const auto space = key == GLFW_KEY_SPACE;
    const auto lsh = key == GLFW_KEY_LEFT_SHIFT;

    camera_pos += TRANSLATION * (float(w-s)*camera.direction + float(a-d)*right_vec + float(space-lsh)*camera.up);
    LOG(camera_pos);
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
