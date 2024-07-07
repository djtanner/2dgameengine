#include "./LevelLoader.h"
#include "./Game.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Logger/Logger.h"
#include <sol/sol.hpp>

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmmitterComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/UILabelComponent.h"
#include "../Components/HealthComponent.h"

LevelLoader::LevelLoader()
{
    Logger::Log("LevelLoader constructor called");
}

LevelLoader::~LevelLoader()
{
    Logger::Log("LevelLoader destructor called");
}

// parse the tilemap file into a 2d vector to get the values
void LevelLoader::parseFile(std::string filename, std::vector<std::vector<int>> &tilemap)
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

void LevelLoader::LoadLevel(SDL_Renderer *renderer, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, int level)
{

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tilemap-sheet", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "projectile", "./assets/images/bullet.png");
    assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
    assetStore->AddFont("arial-font", "./assets/fonts/arial.ttf", 16);
    assetStore->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 24);

    std::vector<std::vector<int>> tilemap;
    // parse the tilemap file and create a map of the tiles
    parseFile("./assets/tilemaps/jungle.map", tilemap);
    int mapNumCols = tilemap[0].size();
    int mapNumRows = tilemap.size();

    //  create entities for each tile in the map
    for (int i = 0; i < mapNumRows; i++)
    {
        for (int j = 0; j < mapNumCols; j++)
        {
            int val = tilemap[i][j];

            //  get the x and y position of the tile in the tilemap to use as the source rect
            int srcX = (val % 10) * TILE_SIZE;
            int srcY = (val / 10) * TILE_SIZE;

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(j * TILE_SIZE * TILE_SCALE, i * TILE_SIZE * TILE_SCALE), glm::vec2(TILE_SCALE, TILE_SCALE), 0.0, false);
            tile.AddComponent<SpriteComponent>("tilemap-sheet", TILE_SIZE, TILE_SIZE, 0, false, srcX, srcY);
            tile.Group("tiles");
        }
    }

    // get map width and height to limit the camera view
    Game::mapWidth = mapNumCols * TILE_SIZE * TILE_SCALE;
    Game::mapHeight = mapNumRows * TILE_SIZE * TILE_SCALE;

    Entity chopper = registry->CreateEntity();

    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", TILE_SIZE, TILE_SIZE, 2);
    chopper.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
    chopper.AddComponent<AnimationComponent>(2, 5, true);
    chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0.0, -40.0), glm::vec2(40.0, 0.0), glm::vec2(0.0, 40.0), glm::vec2(-40.0, 0.0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, false, 10);
    chopper.Tag("player");

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(150.0, 150.0), glm::vec2(1.0, 1.0), 0.0, false);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", TILE_SIZE, TILE_SIZE, 1);
    tank.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 10000, false, 10);
    tank.AddComponent<HealthComponent>(100);
    tank.Group("enemies");

    tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(250.0, 50.0), glm::vec2(1.0, 1.0), 0.0, false);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 30.0));
    tank.AddComponent<SpriteComponent>("tank-image", TILE_SIZE, TILE_SIZE, 1);
    tank.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 10000, false, 10);
    tank.AddComponent<HealthComponent>(100);
    tank.Group("enemies");

    /*Entity tankLabel = registry->CreateEntity();
    tankLabel.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0, true);
    tankLabel.AddComponent<UILabelComponent>("Health: " + std::to_string(tank.GetComponent<HealthComponent>().health), "arial-font", SDL_Color{255, 255, 255, 255}, tank.GetId());
    tankLabel.Group("UIHealth");*/

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(200.0, 10.0), glm::vec2(1.0, 1.0), 0.0, false);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", TILE_SIZE, TILE_SIZE, 1);
    truck.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 10000, false, 10);
    truck.AddComponent<HealthComponent>(100);
    truck.Group("enemies");

    for (int i = 0; i < 15; i++)
    {
        Entity tree = registry->CreateEntity();
        tree.AddComponent<TransformComponent>(glm::vec2(rand() % Game::mapWidth, rand() % Game::mapHeight), glm::vec2(1.0, 1.0), 0.0, false);
        tree.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
        tree.AddComponent<SpriteComponent>("tree-image", TILE_SIZE, TILE_SIZE, 1);
        tree.AddComponent<BoxColliderComponent>(TILE_SIZE, TILE_SIZE);
        tree.Group("obstacles");
    }

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0, true);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", TILE_SIZE * 2, TILE_SIZE * 2, 1, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);
}
