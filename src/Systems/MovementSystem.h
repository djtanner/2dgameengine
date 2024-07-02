#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"
#include "../Components/ProjectileComponent.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event)
    {
        if (event.entity1.BelongsToGroup("obstacles") && event.entity2.BelongsToGroup("enemies"))
        {

            event.entity2.GetComponent<RigidBodyComponent>().velocity.x *= -1;
            event.entity2.GetComponent<RigidBodyComponent>().velocity.y *= -1;
        }
        else if (event.entity2.BelongsToGroup("obstacles") && event.entity1.BelongsToGroup("obstacles"))
        {
            event.entity1.GetComponent<RigidBodyComponent>().velocity.x *= -1;
            event.entity1.GetComponent<RigidBodyComponent>().velocity.y *= -1;
        }
    }

    void Update(double deltaTime)
    {

        for (auto entity : GetSystemEntities())
        {
            auto &transform = entity.GetComponent<TransformComponent>();
            const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidbody.velocity.x * deltaTime;
            transform.position.y += rigidbody.velocity.y * deltaTime;

            // Logger::Log("Entity moved to: " + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y));

            // kill entity if it goes outside of map bounds
            if (transform.position.x < 0 || transform.position.x > Game::mapWidth || transform.position.y < 0 || transform.position.y > Game::mapHeight)
            {
                if (!entity.HasTag("player"))
                    entity.Kill();
            }
        }
    }
};