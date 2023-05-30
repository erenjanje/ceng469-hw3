#pragma once

#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include "things.hpp"
#include <vector>
#include <glm/ext.hpp>

class Object {
    unsigned vao = 0;
    std::size_t index_count = 0;
    glm::vec3 translation = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 scaling = glm::vec3{1.0f, 1.0f, 1.0f};
    glm::mat4 model = glm::mat4(1.0f);
public:
    Object();
    Object(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<GLuint>& indices);
    void draw(ShaderProgram& program, const PerspectiveCamera& camera);
    Object& translate(const glm::vec3 distances);
    Object& rotate(const glm::vec3 angles);
    Object& scale(const glm::vec3 ratios);
    
    glm::vec3& get_translation();
    glm::vec3& get_rotation();
    glm::vec3& get_scale();
    glm::mat4& get_model();
};
