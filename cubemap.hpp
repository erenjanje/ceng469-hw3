#pragma once

#include "shader.hpp"
#include <array>
#include <string>
#include <glm/ext.hpp>

class Cubemap {
    unsigned texture = 0;
    unsigned sampler = 0;
public:
    Cubemap();
    Cubemap(const std::array<std::string, 6>& files);
    
    void bind() const;
    void unbind() const;
    GLuint get_sampler() const;
};
