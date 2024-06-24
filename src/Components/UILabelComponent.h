#pragma once

#include <string>
#include <SDL2/SDL.h>

struct UILabelComponent
{
    std::string text;
    std::string assetId;
    SDL_Color color;

    UILabelComponent(std::string text = "", std::string assetId = "", SDL_Color color = {255, 255, 255})
    {
        this->text = text;
        this->assetId = assetId;
        this->color = color;
    }
};