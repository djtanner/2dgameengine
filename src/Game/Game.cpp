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
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game()
{
    isRunning = false;
    registry = std::make_unique<Registry>();
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

            break;
        }
    }
}

void Game::Setup()
{

    // Add the systems that need to be processed
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // todo:
    Entity tank = registry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
    tank.AddComponent<SpriteComponent>(10, 10);

    //  tank.AddComponent<VelocityComponent>();
    //  tank.AddComponent<SpriteComponent>("./assets/images/tank.png");
    //  tank.AddComponent<BoxColliderComponent>();
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
    registry->GetSystem<RenderSystem>().Update(renderer);
    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    isRunning = false;
}