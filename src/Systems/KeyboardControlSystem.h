#pragma once

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem()
    {
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<KeyboardControlComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPressedEvent &event)
    {
        // change the sprite and velocity
        for (auto entity : GetSystemEntities())
        {
            auto &sprite = entity.GetComponent<SpriteComponent>();
            auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
            auto &keyboardControl = entity.GetComponent<KeyboardControlComponent>();

            if (event.key == SDLK_UP)
            {
                sprite.srcRect.y = sprite.height * 0;
                rigidBody.velocity = keyboardControl.upVelocity;
            }

            else if (event.key == SDLK_RIGHT)
            {
                sprite.srcRect.y = sprite.height * 1;
                rigidBody.velocity = keyboardControl.rightVelocity;
            }
            else if (event.key == SDLK_DOWN)
            {
                sprite.srcRect.y = sprite.height * 2;
                rigidBody.velocity = keyboardControl.downVelocity;
            }
            else if (event.key == SDLK_LEFT)
            {
                sprite.srcRect.y = sprite.height * 3;
                rigidBody.velocity = keyboardControl.leftVelocity;
            }
        }
    }

    void Update()
    {
    }
};