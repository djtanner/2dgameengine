#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Logger/Logger.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<BoxColliderComponent>();
    }

    void Update(std::unique_ptr<EventBus> &eventBus)
    {
        auto entities = GetSystemEntities();
        for (int i = 0; i < entities.size(); i++)
        {
            auto &entity1 = entities[i];
            auto &box1 = entity1.GetComponent<BoxColliderComponent>();
            auto &transform1 = entity1.GetComponent<TransformComponent>();
            for (int j = i + 1; j < entities.size(); j++)
            {
                auto &entity2 = entities[j];
                auto &box2 = entity2.GetComponent<BoxColliderComponent>();
                auto &transform2 = entity2.GetComponent<TransformComponent>();
                if (CheckCollision(box1, transform1, box2, transform2))
                {
                    // Logger::Log("Entity " + std::to_string(entity1.GetId()) + " collided wih entity " + std::to_string(entity2.GetId()));

                    // emit an event
                    eventBus->EmitEvent<CollisionEvent>(entity1, entity2);
                }
            }
        }
    }

    bool CheckCollision(BoxColliderComponent &box1, TransformComponent &transform1, BoxColliderComponent &box2, TransformComponent &transform2)
    {
        return transform1.position.x + box1.offset.x + box1.width >= transform2.position.x + box2.offset.x &&
               transform2.position.x + box2.offset.x + box2.width >= transform1.position.x + box1.offset.x &&
               transform1.position.y + box1.offset.y + box1.height >= transform2.position.y + box2.offset.y &&
               transform2.position.y + box2.offset.y + box2.height >= transform1.position.y + box1.offset.y;
    }
};