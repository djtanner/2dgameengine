#pragma once

#include <vector>
#include <bitset>

const unsigned int MAX_COMPONENTS = 32;

/* Signature
We use a bitset of 1s and 0s to keep track of which components an entity has.
Also helps keep track of which entities a given system should process.
*/
typedef std::bitset<MAX_COMPONENTS> Signature;

class Component
{
};

class Entity
{
private:
    int id;

public:
    Entity(int id) : id(id){};
    int GetId() const;
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
    Signature &GetComponentSignature() const;
};
class Registry
{
};