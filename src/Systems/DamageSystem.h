#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"
#include "../Components/HealthComponent.h"

class DamageSystem : public System
{
public:
    DamageSystem()
    {
        RequireComponent<BoxColliderComponent>();
        RequireComponent<HealthComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event)
    {
        Logger::Log("Entity " + std::to_string(event.entity1.GetId()) + " collided with entity " + std::to_string(event.entity2.GetId()));

        if (event.entity2.BelongsToGroup("projectiles") && !event.entity1.BelongsToGroup("projectiles"))
        {
            OnProjectileHit(event.entity1, event.entity2);
        }
        if (event.entity1.BelongsToGroup("projectiles") && !event.entity2.BelongsToGroup("projectiles"))
        {
            OnProjectileHit(event.entity2, event.entity1);
        }
    }

    void OnProjectileHit(Entity &entity, Entity &projectile)
    {
        auto &health = entity.GetComponent<HealthComponent>();
        auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly && projectileComponent.ownerEntityId != entity.GetId())
        {
            Logger::Err("Entity " + std::to_string(entity.GetId()) + " was hit by projectile " + std::to_string(projectile.GetId()));
            health.health -= projectileComponent.damage;

            if (health.health <= 0)
            {
                entity.Kill();
            }
            projectile.Kill();
        }
    }

    void Update()
    {
    }
};