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
        static float enemyRotation = 0;
        static float enemySpeedX = 0.0f;
        static float enemySpeedY = 0.0f;
        static float scaleX = 1.0;
        static float scaleY = 1.0;

        // Projectile Emitter variables
        static bool emitBullets;
        static float projectileVelocityX;
        static float projectileVelocityY;
        static int frequency;
        static int projectileDuration; // ms to keep alive
        static bool isFriendly;
        static int hitPercentageDamage;

        const char *sprites[] = {"tank-image", "truck-image"};
        static int selectedSprite = 0;

        // ImGui::SeparatorText("Enemy Sprite");
        if (ImGui::CollapsingHeader("Enemy Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Combo("Sprite", &selectedSprite, sprites, IM_ARRAYSIZE(sprites));
        }

        //  ImGui::SeparatorText("Enemy Position");
        if (ImGui::CollapsingHeader("Enemy Position", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputInt("X Position", &enemyXPos);
            ImGui::InputInt("Y Position", &enemyYPos);
            ImGui::InputFloat("Rotation", &enemyRotation);
            ImGui::InputFloat("Scale X", &scaleX);
            ImGui::InputFloat("Scale Y", &scaleY);
        }
        // ImGui::SeparatorText("Enemy Speed");
        if (ImGui::CollapsingHeader("Enemy Speed", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat("Speed X", &enemySpeedX);
            ImGui::InputFloat("Speed Y", &enemySpeedY);
        }

        // ImGui::SeparatorText("Emit Bullets");
        if (ImGui::CollapsingHeader("Emit Bullets", ImGuiTreeNodeFlags_DefaultOpen))
        {

            ImGui::Checkbox("Emit Bullets", &emitBullets);
            ImGui::InputFloat("Bullet Velocity X", &projectileVelocityX);
            ImGui::InputFloat("Bullet Velocity Y", &projectileVelocityY);
            ImGui::InputInt("Frequency ms", &frequency);
            ImGui::InputInt("Duration ms", &projectileDuration);
            ImGui::Checkbox("Is Friendly", &isFriendly);
            ImGui::InputInt("Hit Percentage Damage", &hitPercentageDamage);
        }
        if (ImGui::Button("Spawn Enemy"))
        {
            Entity enemy = registry->CreateEntity();
            enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(scaleX, scaleY), enemyRotation, false);
            enemy.AddComponent<RigidBodyComponent>(glm::vec2(enemySpeedX, enemySpeedY));
            enemy.AddComponent<SpriteComponent>(sprites[selectedSprite], TILE_SIZE, TILE_SIZE, 2);
            enemy.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);

            if (emitBullets)
            {
                enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projectileVelocityX, projectileVelocityY), frequency, projectileDuration, isFriendly, hitPercentageDamage);
            }

            enemy.AddComponent<HealthComponent>(100);
            enemy.Group("enemies");
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