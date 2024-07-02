#pragma once

#include <glm/glm.hpp>
#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent
{
    std::string assetId;
    int width;
    int height;
    int zIndex;
    SDL_Rect srcRect;
    bool isFixed;
    SDL_RendererFlip flip;

    SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->srcRect = {srcRectX, srcRectY, width, height};
        this->zIndex = zIndex;
        this->isFixed = isFixed;
        this->flip = SDL_FLIP_NONE;
    }
};