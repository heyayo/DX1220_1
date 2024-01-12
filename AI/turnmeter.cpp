#include "turnmeter.hpp"
#include "Application.h"

void TurnMeter::NextTurn()
{
    ++_turn;
    _turn %= _actors.size();

    ThisTurn()->think();

    if (ThisTurn()->action.has_value())
    {
        auto& action = ThisTurn()->action.value();
        if (action.destination.has_value())
        {
            _course.clear();
            _maze.pathfind(ThisTurn()->owner,action.destination.value(),_course);
        }
    } else
        NextTurn(); // Skip Turn if they don't think
}

Actor *TurnMeter::ThisTurn()
{ return _actors[_turn]; }

bool TurnMeter::doTurn()
{
    static double timer = 0.0;

    auto& action = ThisTurn()->action.value();

    timer += Application::DELTATIME;
    if (timer > 0.1f && !_course.empty())
    {
        timer = 0.f;
        _maze.teleportEntity(ThisTurn()->owner,*(_course.end()-1));
        _course.erase(_course.end()-1);
        return false;
    }

    if (action.target)
    {
        vec2 a = _maze.getEntityPosition(ThisTurn()->owner);
        vec2 b = _maze.getEntityPosition(action.target);
        vec2 diff;
        diff.first = b.first - a.first;
        diff.second = b.second - a.second;
        diff.first = std::abs(diff.first);
        diff.second = std::abs(diff.second);

        if (diff.first <= 1 || diff.second <= 1)
        {
            // Hard Code loss condition since the enemies only target the player
            // TODO LOSE GAME
            std::cout << "Player Caught" << std::endl;
        }
    }

    return true;
}

TurnMeter::TurnMeter(Maze& maze) : _maze(maze) {}

Actor::Actor(EntityLite *o)
{
    owner = o;
}
