#pragma once

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem()
    {
        RequireComponent<SpriteComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent &event)
    {
        std::string key = std::to_string(event.key);
        std::string keySymbol(1, event.key);

        Logger::Log("Key pressed: " + key + " (" + keySymbol + ")");
    }

    void Update()
    {
    }
};