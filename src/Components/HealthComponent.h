#pragma once

struct HealthComponent
{
    int health;

    HealthComponent(int health = 100)
    {
        this->health = health;
    }
};