#pragma once

#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <iostream>

class AnimationSystem : public System
{
public:
    AnimationSystem()
    {
        RequireComponent<SpriteComponent>();
        RequireComponent<AnimationComponent>();
    }

    void Update()
    {

        for (auto entity : GetSystemEntities())
        {
            auto &sprite = entity.GetComponent<SpriteComponent>();
            auto &animation = entity.GetComponent<AnimationComponent>();

            animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;

            sprite.srcRect.x = sprite.width * animation.currentFrame;
        }
    }
};
