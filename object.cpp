#include "object.hpp"
#include <cstdint>
#include "utils.hpp"
#include <glm/ext.hpp>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <sstream>
#include <array>

Object::Object() {
    vao = 0;
    index_count = 0;
}

Object::Object(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<GLuint>& indices) {
    const auto vertex_size = vertices.size()*sizeof(float);
    const auto normal_size = normals.size()*sizeof(float);
    const auto index_size = indices.size()*sizeof(float);
    unsigned vertex_array, index_array;

    glGenVertexArrays(1, &vao);
    check_gl("VAO");
    LOG(vao);
    glGenBuffers(1, &vertex_array);
    check_gl("VAO");
    glGenBuffers(1, &index_array);
    check_gl("VAO");

    glBindVertexArray(vao);
    check_gl("VAO");
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array);
    check_gl("VAO");
    glBufferData(GL_ARRAY_BUFFER, vertex_size+normal_size, nullptr, GL_STATIC_DRAW);
    check_gl("VAO");
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_size, vertices.data());
    check_gl("VAO");
    glBufferSubData(GL_ARRAY_BUFFER, vertex_size, normal_size, normals.data());
    check_gl("VAO");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array);        
    check_gl("VAO");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, indices.data(), GL_STATIC_DRAW);
    check_gl("VAO");
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    check_gl("VAO");
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    check_gl("VAO");
    glEnableVertexAttribArray(0);
    check_gl("VAO");
    glEnableVertexAttribArray(1);
    check_gl("VAO");
    
    glBindVertexArray(0);
    check_gl("VAO");

    index_count = indices.size();
}

void Object::draw(ShaderProgram& program, const PerspectiveCamera& camera) {
    glm::mat4 model = glm::scale(glm::mat4(1.0f), scaling);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3{0.0f, 0.0f, 1.0f});
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3{0.0f, 1.0f, 0.0f});
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3{1.0f, 0.0f, 0.0f});

    program.use();
    check_gl("Draw");
    program.set("model", model);
    check_gl("Draw");
    program.set("view", glm::lookAt(camera.position, camera.position + camera.direction, camera.up));
    check_gl("Draw");
    program.set("projection", glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, camera.near, camera.far));
    check_gl("Draw");
    program.set("camera_position", camera.position);
    check_gl("Draw");
    glBindVertexArray(vao);
    check_gl("Draw");
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (void*)0);
    check_gl("Draw");
    glBindVertexArray(0);
}

Object& Object::translate(const glm::vec3 distances) {
    translation += distances;
    return *this;
}

Object& Object::rotate(const glm::vec3 angles) {
    rotation += angles;
    return *this;
}

Object& Object::scale(const glm::vec3 ratios) {
    scaling += ratios;
    return *this;
}

glm::vec3& Object::get_translation() {
    return translation;
}

glm::vec3& Object::get_rotation() {
    return rotation;
}

glm::vec3& Object::get_scale() {
    return scaling;
}
