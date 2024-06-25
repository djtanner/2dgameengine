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
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Window A");
        ImGui::Text("This is window A");
        ImGui::End();

        ImGui::Begin("Window B");
        ImGui::Text("This is window B");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};