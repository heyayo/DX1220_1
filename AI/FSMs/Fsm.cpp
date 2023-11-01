#include "Fsm.hpp"

FSM::FSM(Entity* o) : owner(o) {}

Entity* FSM::GetOwner()
{ return owner; }
