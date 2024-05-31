#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetStore
{
private:
    std::map<std::string, SDL_Texture *> textures;

    // TODO: create a map for fonts
    // TODO: create a map for audio

public:
    AssetStore();
    ~AssetStore();

    void AddTexture(SDL_Renderer *renderer, const std::string &assetId, const std::string &filePath);
    SDL_Texture *GetTexture(const std::string &assetId);
    void ClearAssets();
};