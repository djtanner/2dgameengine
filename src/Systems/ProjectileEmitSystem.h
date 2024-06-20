#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/ProjectileEmmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Logger/Logger.h"
#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"

class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem()
    {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        // emit a projectile on space key press
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyEvent);
    }

    void OnKeyEvent(KeyPressedEvent &event)
    {
        if (event.key == SDLK_SPACE)
        {
            Logger::Err("Space key pressed");
            for (auto entity : GetSystemEntities())
            {
                if (entity.HasComponent<CameraFollowComponent>()) // identify if the entity is the player
                {
                    auto &emitter = entity.GetComponent<ProjectileEmitterComponent>();
                    const auto transform = entity.GetComponent<TransformComponent>();
                    const auto rigid = entity.GetComponent<RigidBodyComponent>();
                    if (SDL_GetTicks() - emitter.lastEmmissionTime > emitter.frequency)
                    {

                        glm::vec2 projectilePosition = transform.position;
                        if (entity.HasComponent<SpriteComponent>())
                        {
                            const auto sprite = entity.GetComponent<SpriteComponent>();
                            projectilePosition.x += (transform.scale.x * sprite.width / 2);
                            projectilePosition.y += (transform.scale.y * sprite.height / 2);
                        }

                        // shoot the projectile in the direction of the entity
                        glm::vec2 projectileVelocity = emitter.projectileVelocity;
                        int directionX = 0;
                        int directionY = 0;
                        if (rigid.velocity.x > 0)
                        {
                            directionX = 1;
                        }
                        else if (rigid.velocity.x < 0)
                        {
                            directionX = -1;
                        }
                        if (rigid.velocity.y > 0)
                        {
                            directionY = 1;
                        }
                        else if (rigid.velocity.y < 0)
                        {
                            directionY = -1;
                        }
                        projectileVelocity.x *= directionX;
                        projectileVelocity.y *= directionY;

                        // add projectile to the registry
                        auto projectile = entity.registry->CreateEntity();
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0);
                        projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                        projectile.AddComponent<SpriteComponent>("projectile", 4, 4, 4);
                        projectile.AddComponent<BoxColliderComponent>(4, 4);
                        projectile.AddComponent<ProjectileComponent>(emitter.isFriendly, emitter.hitPercentageDamage, emitter.projectileDuration);

                        // Logger::Err("Projectile emitted");

                        emitter.lastEmmissionTime = SDL_GetTicks();
                    }
                }
            }
        }
    }

    void Update(std::unique_ptr<Registry> &registry)
    {

        for (auto entity : GetSystemEntities())
        {
            auto &emitter = entity.GetComponent<ProjectileEmitterComponent>();
            const auto transform = entity.GetComponent<TransformComponent>();

            if (emitter.frequency == 0)
                continue;

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
                projectile.AddComponent<ProjectileComponent>(emitter.isFriendly, emitter.hitPercentageDamage, emitter.projectileDuration);

                // Logger::Err("Projectile emitted");

                emitter.lastEmmissionTime = SDL_GetTicks();
            }
        }
    }
};