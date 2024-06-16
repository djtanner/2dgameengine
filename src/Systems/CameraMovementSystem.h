#pragma once

#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL2/SDL.h>

class CameraMovementSystem : public System
{
public:
    CameraMovementSystem()
    {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Rect &camera)
    {
        auto entities = GetSystemEntities();
        for (auto entity : entities)
        {

            auto &cameraTransform = entity.GetComponent<TransformComponent>();

            if (cameraTransform.position.x + (camera.w / 2) < Game::mapWidth)
            {
                camera.x = cameraTransform.position.x - (Game::windowWidth / 2);
            }
            if (cameraTransform.position.y + (camera.h / 2) < Game::mapHeight)
            {
                camera.y = cameraTransform.position.y - (Game::windowHeight / 2);
            }

            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = camera.x > camera.w ? camera.w : camera.x;
            camera.y = camera.y > camera.h ? camera.h : camera.y;
        }
    }
};