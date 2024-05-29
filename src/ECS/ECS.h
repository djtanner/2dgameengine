#pragma once

#include <vector>
#include <bitset>

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
class Registry
{
};

template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}