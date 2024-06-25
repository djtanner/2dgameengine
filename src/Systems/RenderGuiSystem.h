#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <SDL2/SDL.h>

class RenderGuiSystem : public System
{
public:
    RenderGuiSystem() = default;

    void Update(SDL_Renderer *renderer)
    {
        ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Window A");
        ImGui::Text("This is window A");
        ImGui::End();

        ImGui::Begin("Window B");
        ImGui::Text("This is window B");
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