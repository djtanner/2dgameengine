#pragma once

#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>
#include "../Logger/Logger.h"

const unsigned int MAX_COMPONENTS = 32;

/* Signature
We use a bitset of 1s and 0s to keep track of which components an entity has.
Also helps keep track of which entities a given system should process.
*/
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent
{
protected:
    static int nextId;
};

// Use a template class to assign a unique ID to each component type
template <typename T>
class Component : public IComponent
{
    // Returns the unique ID of the Component type<T>
public:
    static int GetId()
    {
        static int id = nextId++;
        return id;
    }
};

class Entity
{
private:
    int id;

public:
    Entity(int id) : id(id){};

    int GetId() const;

    bool operator==(const Entity &other) const
    {
        return id == other.id;
    }

    bool operator!=(const Entity &other) const
    {
        return id != other.id;
    }

    bool operator<(const Entity &other) const
    {
        // compare entities based on their IDs
        return this->id < other.id;
    }

    Entity &operator=(const Entity &other)
    {
        id = other.id;
        return *this;
    }

    template <typename TComponent, typename... TArgs>
    void AddComponent(TArgs &&...args);

    template <typename TComponent>
    void RemoveComponent();

    template <typename TComponent>
    void HasComponent() const;

    template <typename TComponent>
    TComponent &GetComponent() const;
    // Keep a pointer to the entity's owner registry, using a forward declaration
    class Registry *registry;
};

/*
System
The system processes entities that contain a specific signature.
*/

class System
{
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature &GetComponentSignature() const;

    // defines the component type T required for entity to be added to system
    template <typename TComponent>
    void RequireComponent();
};

/*A pool is just a vector of objects of type T*/

// use IPool as a base class that is abstract so we don't have to specify the type of the pool
class IPool
{
public:
    virtual ~IPool() = default;
};

template <typename T>
class Pool : public IPool
{
private:
    std::vector<T> data;

public:
    Pool(int size = 100) { Resize(size); }
    virtual ~Pool() = default;

    bool IsEmpty() const { return data.empty(); }
    int GetSize() const { return data.size(); }
    void Resize(int size) { data.resize(size); }
    void Clear() { data.clear(); }
    void Add(const T &component) { data.push_back(component); }
    void Set(int index, const T &object) { data[index] = object; }
    T &Get(int index) { return static_cast<T &>(data[index]); }
    T &operator[](int index) { return data[index]; }
};

/*Manages the creation and destruction of entites, as well as adding systems and adding componenets to entities*/
class Registry
{
private:
    int numEntities = 0;

    // need sets of entities so can update at end of frame update in game loop
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

    // Vector of component pools
    // each pool contains all the data for a certain component type, each pool will be different types so use the abstract IPool
    // vector index is componenet ID, pool index = entity ID
    std::vector<std::shared_ptr<IPool>> componentPools;

    // Vector of component signatures
    // each signature represents the components an entity has
    // vector index = entity ID
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

public:
    Registry() = default;

    // use unique_ptr for registry so it will automatically deallocate when out of scope
    ~Registry()
    {
        Logger::Log("Registry was destructed");
    }
    void Update();

    // Management of ECS
    Entity CreateEntity();
    void KillEntity(Entity entity);
    void AddEntityToSystem(Entity entity);

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Components
    ////////////////////////////////////////////////////////////////////////////////////////////
    // &&args is a rvalue reference, used to pass arguments to the constructor of the component
    template <typename TComponent, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&...args);

    template <typename TComponent>
    void RemoveComponent(Entity entity);

    template <typename TComponent>
    bool HasComponent(Entity entity) const;

    template <typename TComponent>
    TComponent &GetComponent(Entity entity) const;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // Systems
    ////////////////////////////////////////////////////////////////////////////////////////////
    template <typename TSystem, typename... TArgs>
    void AddSystem(TArgs &&...args);

    template <typename TSystem>
    void RemoveSystem();

    template <typename TSystem>
    bool HasSystem() const;

    template <typename TSystem>
    TSystem &GetSystem() const;

    void AddEntityToSystems(Entity entity);
};

/*Implementation of RequireComponent*/
template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

/*Implementation of Registry template functions*/
template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    // Resize componentPools if necessary
    if (componentPools.size() <= componentId)
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    // Resize componentPools[componentId] if necessary
    if (componentPools[componentId] == nullptr)
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(numEntities);
    }

    // Add component to pool

    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);

    // Update entity signature to turn on the bit representing the component
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("component id " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (this->template HasComponent<TComponent>(entity))
    {
        entityComponentSignatures[entityId].set(componentId, false);
    }
}

template <typename TComponent>
TComponent &Registry::GetComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

// Implementation of Systems templates
template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return system != systems.end();
}

template <typename TSystem>
TSystem &Registry::GetSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

/* Implementation of Entity template functions*/
template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&...args)
{
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
void Entity::HasComponent() const
{
    registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent &Entity::GetComponent() const
{
    registry->GetComponent<TComponent>(*this);
}
