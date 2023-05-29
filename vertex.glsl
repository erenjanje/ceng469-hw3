#version 460 core

layout (location = 0) in vec3 model_position;
layout (location = 1) in vec3 model_normal;

out vec4 vertex_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vertex_position = model * vec4(model_position, 1.0f);
    gl_Position = projection * view * vertex_position;
}
