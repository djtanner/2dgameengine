#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>
#include "../AssetStore/AssetStore.h"

class RenderSystem : public System
{
public:
    RenderSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore)
    {
        struct RenderableEntity
        {
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
            bool hasCollider;
        };

        std::vector<RenderableEntity>
            renderableEntities;

        // sort entities by z index
        for (auto entity : GetSystemEntities())
        {
            RenderableEntity renderableEntity;
            renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
            renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
            renderableEntity.hasCollider = entity.HasComponent<BoxColliderComponent>();
            renderableEntities.emplace_back(renderableEntity);

            // draw  box collider

            /* if (entity.HasComponent<BoxColliderComponent>())
             {
                 const auto transform = entity.GetComponent<TransformComponent>();
                 const auto sprite = entity.GetComponent<SpriteComponent>();
                 SDL_Rect collider = {
                     static_cast<int>(transform.position.x + sprite.width / 2 - sprite.width / 2 * transform.scale.x),
                     static_cast<int>(transform.position.y + sprite.height / 2 - sprite.height / 2 * transform.scale.y),
                     static_cast<int>(sprite.width * transform.scale.x),
                     static_cast<int>(sprite.height * transform.scale.y),
                 };
                 SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                 SDL_RenderDrawRect(renderer, &collider);
             }*/
        }

        std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity &a, const RenderableEntity &b)
                  { return a.spriteComponent.zIndex < b.spriteComponent.zIndex; });

        for (auto entity : renderableEntities)
        {
            const auto transform = entity.transformComponent;
            const auto sprite = entity.spriteComponent;

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y),
            };

            SDL_Rect srcRect = sprite.srcRect;

            SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &srcRect, &dstRect, transform.rotation, nullptr, SDL_FLIP_NONE);

            if (entity.hasCollider)
            {

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &dstRect);
            }
        }
    }
};