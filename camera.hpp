#pragma once 

#include <glm/ext.hpp>

extern const int width;
extern const int height;

struct PerspectiveCamera {
    float fov = 45.0f;
    float aspect_ratio = (float)height / (float)width;
    float near = 0.1f;
    float far = 5.0f;
    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 direction = glm::vec3{0.0f, 0.0f, -1.0f};
    glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f};
};
