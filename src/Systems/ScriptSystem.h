#pragma once

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"

class ScriptSystem : public System
{
public:
    ScriptSystem()
    {
        RequireComponent<ScriptComponent>();
    }

    void Update()
    {
        for (auto entity : GetSystemEntities())
        {
            auto &script = entity.GetComponent<ScriptComponent>();
            script.func();
        }
    }
};