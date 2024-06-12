#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"
#include <SDL2/SDL_image.h>

class KeyPressedEvent : public Event
{
public:
    SDL_Keycode key;
    KeyPressedEvent(SDL_Keycode key) : key(key) {}
};