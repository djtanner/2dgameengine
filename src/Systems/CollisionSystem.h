#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Logger/Logger.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<BoxColliderComponent>();
    }

    void Update()
    {
        auto entities = GetSystemEntities();
        for (int i = 0; i < entities.size(); i++)
        {
            auto &entity1 = entities[i];
            auto &box1 = entity1.GetComponent<BoxColliderComponent>();
            for (int j = i + 1; j < entities.size(); j++)
            {
                auto &entity2 = entities[j];
                auto &box2 = entity2.GetComponent<BoxColliderComponent>();
                if (CheckCollision(box1, box2))
                {
                    Logger::Log("Collision Detected");
                }
            }
        }
    }

    bool CheckCollision(BoxColliderComponent &box1, BoxColliderComponent &box2)
    {
        return (box1.offset.x + box1.width >= box2.offset.x &&
                box2.offset.x + box2.width >= box1.offset.x) ||
               (box1.offset.y + box1.height >= box2.offset.y &&
                box2.offset.y + box2.height >= box1.offset.y);
    }
};