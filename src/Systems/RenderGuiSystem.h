#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <SDL2/SDL.h>
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmmitterComponent.h"
#include "../Components/HealthComponent.h"
#include <glm/glm.hpp>
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"

class RenderGuiSystem : public System
{
public:
    RenderGuiSystem() = default;

    void Update(SDL_Renderer *renderer, std::unique_ptr<Registry> &registry)
    {
        ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Spawn Enemies");
        static int enemyXPos = 0;
        static int enemyYPos = 0;
        static float enemySpeedX = 0.0f;
        static float enemySpeedY = 0.0f;

        ImGui::InputInt("X Position", &enemyXPos);
        ImGui::InputInt("Y Position", &enemyYPos);
        // ImGui::InputFloat("Speed X", &enemySpeedX);
        // ImGui::InputFloat("Speed Y", &enemySpeedY);

        if (ImGui::Button("Spawn Enemy"))
        {
            Entity tank = registry->CreateEntity();
            tank.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(1.0, 1.0), 0.0, false);
            tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0));
            tank.AddComponent<SpriteComponent>("tank-image", TILE_SIZE, TILE_SIZE, 2);
            tank.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
            tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 10000, false, 10);
            tank.AddComponent<HealthComponent>(100);
            tank.Group("enemies");
        }
        ImGui::End();

        ImGui::Begin("Logger");
        for (auto &message : Logger::messages)
        {
            if (message.type == LOG_ERROR)
            {
                ImGui::TextColored(red, "%s", message.message.c_str());
            }
            else
            {
                ImGui::TextColored(green, "%s", message.message.c_str());
            }
        }
        ImGui::End();

        // ImGui demo window
        // ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};