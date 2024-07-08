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

void LevelLoader::LoadLevel(sol::state &lua, std::unique_ptr<Registry> &registry, std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer, int levelNumber)
{

    // This checks the syntax of our script, but it does not execute the script
    sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
    if (!script.valid())
    {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Err("Error loading the lua script: " + errorMessage);
        return;
    }

    // Executes the script using the Sol state
    lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

    // Read the big table for the current level
    sol::table level = lua["Level"];

    ////////////////////////////////////////////////////////////////////////////
    // Read the level assets
    ////////////////////////////////////////////////////////////////////////////
    sol::table assets = level["assets"];

    int i = 0;
    while (true)
    {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt)
        {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        if (assetType == "texture")
        {
            assetStore->AddTexture(renderer, assetId, asset["file"]);
            Logger::Log("A new texture asset was added to the asset store, id: " + assetId);
        }
        if (assetType == "font")
        {
            assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("A new font asset was added to the asset store, id: " + assetId);
        }
        i++;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Read the level tilemap information
    ////////////////////////////////////////////////////////////////////////////
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];

    int TILE_SIZE = map["tile_size"];
    double mapScale = map["scale"];

    std::vector<std::vector<int>> tilemap;

    parseFile(mapFilePath, tilemap);

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
            tile.AddComponent<SpriteComponent>(mapTextureAssetId, TILE_SIZE, TILE_SIZE, 0, false, srcX, srcY);
            tile.Group("tiles");
        }
    }
    Game::mapWidth = mapNumCols * TILE_SIZE * mapScale;
    Game::mapHeight = mapNumRows * TILE_SIZE * mapScale;

    ////////////////////////////////////////////////////////////////////////////
    // Read the level entities and their components
    ////////////////////////////////////////////////////////////////////////////
    sol::table entities = level["entities"];
    i = 0;
    while (true)
    {

        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt)
        {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt)
        {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt)
        {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt)
        {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt)
            {
                sol::table transformTable = transform.value();
                glm::vec2 position = {transformTable["position"]["x"], transformTable["position"]["y"]};
                glm::vec2 scale = {transformTable["scale"]["x"], transformTable["scale"]["y"]};
                double rotation = transformTable["rotation"].get_or(0.0);
                bool isFixed = transformTable["isFixed"].get_or(false);
                newEntity.AddComponent<TransformComponent>(position, scale, rotation, isFixed);
            }

            // RigidBody
            sol::optional<sol::table> rigidBody = entity["components"]["rigidbody"];
            if (rigidBody != sol::nullopt)
            {
                sol::table rigidBodyTable = rigidBody.value();
                glm::vec2 velocity = {rigidBodyTable["velocity"]["x"], rigidBodyTable["velocity"]["y"]};
                newEntity.AddComponent<RigidBodyComponent>(velocity);
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt)
            {
                sol::table spriteTable = sprite.value();
                std::string assetId = spriteTable["texture_asset_id"];
                int width = spriteTable["width"];
                int height = spriteTable["height"];
                int zIndex = spriteTable["zIndex"];
                bool isFixed = spriteTable["isFixed"].get_or(false);
                int srcRectX = spriteTable["src_rect_x"];
                int srcRectY = spriteTable["src_rect_y"];
                SDL_RendererFlip flip = spriteTable["flip"].get_or(SDL_FLIP_NONE);
                newEntity.AddComponent<SpriteComponent>(assetId, width, height, zIndex, isFixed, srcRectX, srcRectY, flip);
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt)
            {
                sol::table animationTable = animation.value();
                int numFrames = animationTable["num_frames"];
                int animationSpeed = animationTable["speed_rate"];
                bool isLoop = animationTable["is_loop"].get_or(true);
                newEntity.AddComponent<AnimationComponent>(numFrames, animationSpeed, isLoop);
            }

            // Box Collider
            sol::optional<sol::table> boxCollider = entity["components"]["boxcollider"];
            if (boxCollider != sol::nullopt)
            {
                sol::table boxColliderTable = boxCollider.value();
                int width = boxColliderTable["width"];
                int height = boxColliderTable["height"];
                int offsetX = boxColliderTable["offset"]["x"].get_or(0);
                int offsetY = boxColliderTable["offset"]["y"].get_or(0);
                glm::vec2 offset = {offsetX, offsetY};
                newEntity.AddComponent<BoxColliderComponent>(width, height, offset);
            }

            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt)
            {
                sol::table healthTable = health.value();
                int healthValue = healthTable["health_percentage"];
                newEntity.AddComponent<HealthComponent>(healthValue);
            }

            // Projectile Emitter

            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt)
            {
                sol::table projectileEmitterTable = projectileEmitter.value();
                glm::vec2 projectileVelocity = {projectileEmitterTable["projectile_velocity"]["x"], projectileEmitterTable["projectile_velocity"]["y"]};
                int frequency = static_cast<int>(projectileEmitterTable["repeat_frequency"].get_or(1)) * 1000;
                int projectileDuration = static_cast<int>(projectileEmitterTable["projectile_duration"].get_or(10)) * 1000;
                int hitPercentageDamage = static_cast<int>(projectileEmitterTable["hit_percentage_damage"].get_or(10));
                bool isFriendly = projectileEmitterTable["friendly"].get_or(false);

                newEntity.AddComponent<ProjectileEmitterComponent>(projectileVelocity, frequency, projectileDuration, isFriendly, hitPercentageDamage);
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt)
            {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // Keyboard Control
            sol::optional<sol::table> keyboardControl = entity["components"]["keyboard_controller"];
            if (keyboardControl != sol::nullopt)
            {
                sol::table keyboardControlTable = keyboardControl.value();
                glm::vec2 up = {keyboardControlTable["up_velocity"]["x"], keyboardControlTable["up_velocity"]["y"]};
                glm::vec2 right = {keyboardControlTable["right_velocity"]["x"], keyboardControlTable["right_velocity"]["y"]};
                glm::vec2 down = {keyboardControlTable["down_velocity"]["x"], keyboardControlTable["down_velocity"]["y"]};
                glm::vec2 left = {keyboardControlTable["left_velocity"]["x"], keyboardControlTable["left_velocity"]["y"]};

                newEntity.AddComponent<KeyboardControlComponent>(up, right, down, left);
            }
            /*




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
                      */
        }
        i++;
    }
}
