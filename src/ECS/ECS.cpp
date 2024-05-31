#include "ECS.h"
#include "../Logger/Logger.h"

// initialize static member variable
int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void System::AddEntityToSystem(Entity entity)
{
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    entities.erase(std::remove(entities.begin(), entities.end(), entity),
                   entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
    return entities;
}

const Signature &System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId;

    entityId = numEntities++;
    Entity entity(entityId);
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with ID: " + std::to_string(entityId));
    return entity;
}

void Registry::Update()
{
    // Add entities to systems
}

// Adds an entity to the System if the entity contains all of the required components
void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const auto &entitySignature = entityComponentSignatures[entityId];

    for (auto &system : systems)
    {
        const auto &systemComponentSignature = system.second->GetComponentSignature();

        bool isMatching = (entitySignature & systemComponentSignature) == systemComponentSignature;

        if (isMatching)
        {
            system.second->AddEntityToSystem(entity);
        }
    }
}