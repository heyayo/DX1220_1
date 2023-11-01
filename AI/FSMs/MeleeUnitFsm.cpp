#include "MeleeUnitFsm.hpp"

MeleeUnitFSM::MeleeUnitFSM(Entity* o) : FSM(o)
{
    ChangeState(STATES::SEARCHING);
}

void MeleeUnitFSM::Update(double deltaTime)
{

}
