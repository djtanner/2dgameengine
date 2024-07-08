#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"
#include <glm/glm.hpp>

glm::vec2 GetEntityPosition(Entity entity)
{
    auto &transform = entity.GetComponent<TransformComponent>();
    return transform.position;
}

void SetEntityPosition(Entity entity, double x, double y)
{
    if (entity.HasComponent<TransformComponent>())
    {

        auto &transform = entity.GetComponent<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    }
    else
    {
        Logger::Err("Entity does not have TransformComponent");
    }
}

class ScriptSystem : public System
{
public:
    ScriptSystem()
    {
        RequireComponent<ScriptComponent>();
    }

    void CreateLuaBindings(sol::state &lua)
    {
        // create an entity usertype
        lua.new_usertype<Entity>("entity",
                                 "get_id", &Entity::GetId,
                                 "has_tag", &Entity::HasTag,
                                 "destroy", &Entity::Kill,
                                 "belongs_to_group", &Entity::BelongsToGroup);

        lua.new_usertype<TransformComponent>("TransformComponent",
                                             "position", &TransformComponent::position,
                                             "scale", &TransformComponent::scale,
                                             "rotation", &TransformComponent::rotation,
                                             "isFixed", &TransformComponent::isFixed);

        lua.set_function("get_position", GetEntityPosition);
        lua.set_function("set_position", SetEntityPosition);
    }

    void Update(double deltaTime, int ellapsedTime)
    {
        for (auto entity : GetSystemEntities())
        {
            auto &script = entity.GetComponent<ScriptComponent>();
            script.func(entity, deltaTime, ellapsedTime);
        }
    }
};