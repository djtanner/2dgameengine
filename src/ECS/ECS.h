#pragma once

#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>

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

// Used to assign a unique ID to each component type
template <typename T>
class Component : public IComponent
{
    // Returns the unique ID of the Component type<T>
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

    Entity &operator=(const Entity &other)
    {
        id = other.id;
        return *this;
    }
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

    // Vector of component pools
    // each pool contains all the data for a certain component type, each pool will be different types so use the abstract IPool
    // vector index is componenet ID, pool index = entity ID
    std::vector<IPool *> componentPools;

    // Vector of component signatures
    // each signature represents the components an entity has
    // vector index = entity ID
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, System *> systems;

public:
    Registry() = default;

    // Management of ECS
    Entity CreateEntity();
    void KillEntity(Entity entity);
    void AddSystem(System *system);
    void AddComponent(Entity entity, IComponent *component);
};

/*Implementation of RequireComponent*/
template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}