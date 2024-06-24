#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
    glm::vec2 position;
    glm::vec2 scale;
    double rotation;
    bool isFixed;

    TransformComponent(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), double rotation = 0.0, bool isFixed = false)
    {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
        this->isFixed = isFixed;
    }
};