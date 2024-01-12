#ifndef DX1220_TURNMETER_HPP
#define DX1220_TURNMETER_HPP

#include "entitylite.hpp"
#include "FSMs/statemachine.hpp"
#include "maze.hpp"

#include <vector>
#include <optional>

struct TurnAction
{
    std::optional<vec2> destination;
    float speed{};
    EntityLite* target = nullptr;
};

struct Actor
{
    EntityLite* owner;
    std::optional<TurnAction> action;

    Actor(EntityLite* o);

    virtual void think() = 0;
};

class TurnMeter
{
    Maze& _maze;
    std::vector<Actor*> _actors;

    size_t _turn;

    void NextTurn();
    Actor* ThisTurn();

    std::vector<vec2> _course;

public:

    TurnMeter(Maze& maze);

    bool doTurn(); // Call in Update()
};

#endif
