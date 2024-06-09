#include "ECS.h"
#include "../Logger/Logger.h"

// initialize static member variable
int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void Entity::Kill()
{
    registry->KillEntity(*this);
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

    if (freeIds.size() > 0)
    {

        entityId = freeIds.front();
        freeIds.pop();
    }
    else
    {
        entityId = numEntities++;
        // Make sure the entityComponentSignatures vector can accommodate the new entity
        if (entityId >= entityComponentSignatures.size())
        {
            entityComponentSignatures.resize(entityId + 1);
        }
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with ID: " + std::to_string(entityId));
    return entity;
}

void Registry::KillEntity(Entity entity)
{

    // Remove the entity from the set of entities to be destroyed
    entitiesToBeKilled.insert(entity);
}

void Registry::Update()
{
    // Add entities to the correct systems

    for (auto entity : entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }

    entitiesToBeAdded.clear();

    for (auto entity : entitiesToBeKilled)
    {
        RemoveEntityFromSystems(entity);
        freeIds.push(entity.GetId());

        entityComponentSignatures[entity.GetId()].reset();
    }

    entitiesToBeKilled.clear();
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

// Removes an entity from the System
void Registry::RemoveEntityFromSystems(Entity entity)
{
    for (auto &system : systems)
    {
        system.second->RemoveEntityFromSystem(entity);
    }
}