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

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore, bool renderColliders, SDL_Rect &camera)
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
        }

        std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity &a, const RenderableEntity &b)
                  { return a.spriteComponent.zIndex < b.spriteComponent.zIndex; });

        for (auto entity : renderableEntities)
        {
            const auto transform = entity.transformComponent;
            const auto sprite = entity.spriteComponent;

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x - (!sprite.isFixed ? camera.x : 0)), // shift rendering sprites by camera position
                static_cast<int>(transform.position.y - (!sprite.isFixed ? camera.y : 0)),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y),
            };

            SDL_Rect srcRect = sprite.srcRect;

            SDL_RenderCopyEx(renderer, assetStore->GetTexture(sprite.assetId), &srcRect, &dstRect, transform.rotation, nullptr, SDL_FLIP_NONE);

            if (entity.hasCollider && renderColliders)
            {

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &dstRect);
            }
        }
    }
};