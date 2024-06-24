#pragma once

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL_ttf.h>
#include <string>
#include <SDL2/SDL.h>

class RenderHealthUISystem : public System
{
public:
    RenderHealthUISystem()
    {
        RequireComponent<HealthComponent>();
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore, const SDL_Rect &camera)
    {
        for (auto entity : GetSystemEntities())
        {

            const auto healthComponent = entity.GetComponent<HealthComponent>();
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto spriteComponent = entity.GetComponent<SpriteComponent>();

            std::string text = std::to_string(healthComponent.health) + "%";
            SDL_Color color = {255, 255, 255};

            SDL_Rect r;
            r.x = transform.position.x - camera.x;
            r.y = transform.position.y + TILE_SIZE - camera.y;
            r.w = (30 * (healthComponent.health / 100.0));
            r.h = 8;

            if (healthComponent.health >= 70)
            {
                color = {0, 255, 0};
            }
            else if (healthComponent.health >= 30)
            {
                color = {255, 255, 0};
            }
            else if (healthComponent.health < 30)
            {
                color = {255, 0, 0};
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(renderer, &r);
            SDL_Surface *surface = TTF_RenderText_Blended(
                assetStore->GetFont("charriot-font"),
                text.c_str(),
                color);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;
            SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x - 10 - (camera.x)),
                static_cast<int>(transform.position.y - 10 - (camera.y)),
                25,
                25};

            SDL_RenderCopy(renderer, texture, NULL, &dstRect);

            SDL_DestroyTexture(texture);
        }
    }
};
