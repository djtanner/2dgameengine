#include "AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>
AssetStore::AssetStore()

{
    Logger::Log("AssetStore created");
}

AssetStore::~AssetStore()
{
    ClearAssets();
    Logger::Log("AssetStore destroyed");
}

void AssetStore::ClearAssets()
{
    for (auto &texture : textures)
    {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    for (auto &font : fonts)
    {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
}

void AssetStore::AddTexture(SDL_Renderer *renderer, const std::string &assetId, const std::string &filePath)
{
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        Logger::Err("Error loading image: " + filePath);
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        Logger::Err("Error creating texture from surface: " + filePath);
        return;
    }

    SDL_FreeSurface(surface);
    textures.emplace(assetId, texture);
    Logger::Log("Texture loaded: " + filePath);
}

SDL_Texture *AssetStore::GetTexture(const std::string &assetId)
{
    return textures[assetId];
}

void AssetStore::AddFont(const std::string &assetId, const std::string &filePath, int fontSize)
{
    TTF_Font *font = TTF_OpenFont(filePath.c_str(), fontSize);
    if (!font)
    {
        Logger::Err("Error loading font: " + filePath);
        return;
    }

    fonts.emplace(assetId, font);
    Logger::Log("Font loaded: " + filePath);
}

TTF_Font *AssetStore::GetFont(const std::string &assetId)
{
    return fonts[assetId];
}