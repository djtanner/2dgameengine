#pragma once

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>
#include <memory>
#include <sol/sol.hpp>

class LevelLoader
{
public:
    LevelLoader();
    ~LevelLoader();
    void parseFile(std::string filename, std::vector<std::vector<int>> &tilemap);
    void LoadLevel(sol::state &lua, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer, int levelNumber);
};