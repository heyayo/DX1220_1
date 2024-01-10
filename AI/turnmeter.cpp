#include "turnmeter.hpp"
#include "Application.h"

void TurnMeter::NextTurn()
{
    ++_turn;
    _turn %= _actors.size();

    if (ThisTurn()->action.has_value())
    {
        
    }
}

Actor *TurnMeter::ThisTurn()
{ return _actors[_turn]; }

void TurnMeter::doTurn()
{
    static std::vector<vec2> course;

    if (!ThisTurn()->action.has_value())
    {
        NextTurn(); // Skip this turn
        course.clear();
        return;
    }
    auto& action = ThisTurn()->action.value();

    if (action.destination.has_value() && course.empty())
        _maze.pathfind(ThisTurn()->owner,action.destination.value(),course);
}

TurnMeter::TurnMeter(Maze& maze) : _maze(maze) {}
