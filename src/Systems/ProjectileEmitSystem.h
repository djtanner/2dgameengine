#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/ProjectileEmmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Logger/Logger.h"

class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem()
    {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<Registry> &registry)
    {

        for (auto entity : GetSystemEntities())
        {
            auto &emitter = entity.GetComponent<ProjectileEmitterComponent>();
            const auto transform = entity.GetComponent<TransformComponent>();
            if (SDL_GetTicks() - emitter.lastEmmissionTime > emitter.frequency)
            {

                glm::vec2 projectilePosition = transform.position;
                if (entity.HasComponent<SpriteComponent>())
                {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width / 2);
                    projectilePosition.y += (transform.scale.y * sprite.height / 2);
                }
                // add projectile to the registry
                auto projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0);
                projectile.AddComponent<RigidBodyComponent>(emitter.projectileVelocity);
                projectile.AddComponent<SpriteComponent>("projectile", 4, 4, 4);
                projectile.AddComponent<BoxColliderComponent>(4, 4);

                // Logger::Err("Projectile emitted");

                emitter.lastEmmissionTime = SDL_GetTicks();
            }
        }
    }
};