#include "Fsm.hpp"

FSM::FSM(Entity* o, std::vector<Entity*>& opTeam) : oppositeTeam(opTeam), owner(o)
{

}

const std::vector<Entity*> &FSM::GetOppositeTeam()
{
    return oppositeTeam;
}

Entity* FSM::GetOwner()
{
    return owner;
}
