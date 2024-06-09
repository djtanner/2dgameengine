#include "Game.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <SDL2/SDL_image.h>
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../AssetStore/AssetStore.h"
#include <vector>
#include <fstream>
#include <sstream>

Game::Game()
{
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    renderColliders = false;
    Logger::Log("Game constructor called");
}

Game::~Game()
{
    Logger::Log("Game destructor called");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Err("Error initializing SDL");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = 800;  // displayMode.w;
    windowHeight = 600; // displayMode.h;

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        Logger::Err("Error creating SDL window");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        Logger::Err("Error creating SDL renderer");
        return;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    isRunning = true;
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
                return;
            }
            if (sdlEvent.key.keysym.sym == SDLK_d)
            {
                renderColliders = !renderColliders;
            }

            break;
        }
    }
}

// parse the tilemap file into a 2d vector to get the values
void Game::parseFile(std::string filename, std::vector<std::vector<int>> &tilemap)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        Logger::Err("Error opening file: " + filename);
    }

    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string item;
        std::vector<int> row;

        while (std::getline(ss, item, ','))
        {
            row.push_back(std::stoi(item));
        }

        tilemap.push_back(row);
    }

    file.close();
}

void Game::LoadLevel(int level)
{
    // Add the systems that need to be processed
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();

    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tilemap-sheet", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper.png");

    std::vector<std::vector<int>> tilemap;
    // parse the tilemap file and create a map of the tiles
    parseFile("./assets/tilemaps/jungle.map", tilemap);

    //  create entities for each tile in the map
    for (int i = 0; i < tilemap.size(); i++)
    {
        for (int j = 0; j < tilemap[i].size(); j++)
        {
            int val = tilemap[i][j];

            //  get the x and y position of the tile in the tilemap to use as the source rect
            int srcX = (val % 10) * TILE_SIZE;
            int srcY = (val / 10) * TILE_SIZE;

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(j * TILE_SIZE * TILE_SCALE, i * TILE_SIZE * TILE_SCALE), glm::vec2(TILE_SCALE, TILE_SCALE), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-sheet", TILE_SIZE, TILE_SIZE, 0, srcX, srcY);
        }
    }

    /* Entity chopper = registry->CreateEntity();

     chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
     chopper.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
     chopper.AddComponent<SpriteComponent>("chopper-image", TILE_SIZE, TILE_SIZE, 2);
     chopper.AddComponent<AnimationComponent>(2, 5, true);
 */
    Entity tank = registry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(50.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(5.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", TILE_SIZE, TILE_SIZE, 2);
    tank.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", TILE_SIZE, TILE_SIZE, 1);
    truck.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
}

void Game::Setup()
{
    LoadLevel(1);
}

void Game::Update()
{
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    millisecsPreviousFrame = SDL_GetTicks();
    // TODO:
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update();

    //  CollisionSystem.Update();
    //  RenderSystem.Update();
    //  HealthSystem.Update();
    //  AnimationSystem.Update();
    //  AudioSystem.Update();

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // registry->GetSystem<RenderSystem>().Update(renderer, std::make_unique<AssetStore> & assetStore);
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore, renderColliders);
    SDL_RenderPresent(renderer);
}

void Game::Run()
{
    Setup();
    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    isRunning = false;
}