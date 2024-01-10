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
    float damage{};
};

struct Actor
{
    EntityLite* owner;
    StateMachine* sm;
    std::optional<TurnAction> action;
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

    void doTurn(); // Call in Update()
};

#endif
