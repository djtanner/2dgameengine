#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"

class DamageSystem : public System
{
public:
    DamageSystem()
    {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event)
    {
        Logger::Log("Entity " + std::to_string(event.entity1.GetId()) + " collided with entity " + std::to_string(event.entity2.GetId()));
        event.entity1.Kill();
        event.entity2.Kill();
    }

    void Update()
    {
    }
};