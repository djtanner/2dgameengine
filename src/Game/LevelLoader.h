#pragma once

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>
#include <memory>

class LevelLoader
{
public:
    LevelLoader();
    ~LevelLoader();
    void parseFile(std::string filename, std::vector<std::vector<int>> &tilemap);
    void LoadLevel(SDL_Renderer *renderer, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, int level);
};