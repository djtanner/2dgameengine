#pragma once

class MovementSystem : public System
{
public:
    MovementSystem()
    {
        // TODO: RequireComponent<TransformComponent>();
        // TODO: RequireComponent<...>();
    }

    void Update()
    {
        // TODO: update entity position based on its velocity every frame of the game loop

        for (auto entity : GetEntities())
        {
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &velocity = entity.GetComponent<VelocityComponent>();

            transform.position += velocity.value;
        }
    }
};