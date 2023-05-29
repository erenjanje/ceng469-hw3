#version 460 core

in vec4 vertex_position;
out vec4 frag_color;

uniform samplerCube cubemap;
uniform vec3 camera_position;
uniform vec3 camera_direction;

vec3 gradients[16] = {
    vec3(1, 1, 0),
    vec3(-1, 1, 0),
    vec3(1, -1, 0),
    vec3(-1, -1, 0),
    vec3(1, 0, 1),
    vec3(-1, 0, 1),
    vec3(1, 0, -1),
    vec3(-1, 0, -1),
    vec3(0, 1, 1),
    vec3(0, -1, 1),
    vec3(0, 1, -1),
    vec3(0, -1, -1),
    vec3(1, 1, 0),
    vec3(-1, 1, 0),
    vec3(0, -1, 1),
    vec3(0, -1, -1)
};

int table[16] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};

vec3 get_gradient(ivec3 v) {
    int idx = table[abs(v.z) % 16];
    idx = table[abs(v.y + idx) % 16];
    idx = table[abs(v.x + idx) % 16];
    return gradients[idx];
}

void get_corners(out ivec3 corners[8], vec3 position) {
    ivec3 floored = ivec3(floor(position));
    ivec3 ceiled = ivec3(ceil(position));
    corners[0] = ivec3(floored.x, floored.y, floored.z);
    corners[1] = ivec3(floored.x, floored.y,  ceiled.z);
    corners[2] = ivec3(floored.x,  ceiled.y, floored.z);
    corners[3] = ivec3(floored.x,  ceiled.y,  ceiled.z);
    corners[4] = ivec3( ceiled.x, floored.y, floored.z);
    corners[5] = ivec3( ceiled.x, floored.y,  ceiled.z);
    corners[6] = ivec3( ceiled.x,  ceiled.y, floored.z);
    corners[7] = ivec3( ceiled.x,  ceiled.y,  ceiled.z);
}

void main() {
    vec3 ray = vertex_position.xyz - camera_position;
    vec3 abs_ray = abs(ray);
    vec3 uvr = ray / max(abs_ray.x, max(abs_ray.y, abs_ray.z));
    frag_color = texture(cubemap, vec3(-uvr.x, uvr.yz));
    frag_color = vec4(vertex_position.xyz * 0.5f + 0.5f, 1.0f);
}
