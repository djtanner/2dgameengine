#pragma once

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void Update(double deltaTime)
    {
        // TODO: update entity position based on its velocity every frame of the game loop

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