#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

const int FPS = 500;
const int MILLISECS_PER_FRAME = 1000 / FPS;
const int TILE_SIZE = 32;
const int TILE_SCALE = 2;
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;

class Game
{
private:
    bool isRunning;
    int millisecsPreviousFrame = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool renderColliders = false;
    SDL_Rect camera;
    SDL_GLContext GLContext;

    // temp imgui functions
    bool InitImGui();
    void Begin();
    void End();
    void RenderImGui();

    void parseFile(std::string filename, std::vector<std::vector<int>> &tilemap);
    // entity manager
    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus; // keeps track of event subscriptions

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void LoadLevel(int level);
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    inline SDL_GLContext &GetGLContext() { return GLContext; }

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
};

#endif