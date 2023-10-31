#include "MeleeUnitFsm.hpp"

#include "scenea1.hpp"

MeleeUnitFSM::MeleeUnitFSM(Entity* o, std::vector<Entity*>& opTeam) : FSM(o, opTeam)
{
    ChangeState<STATES>(STATES::IDLE);
    _moveSpeed = 3.f;
}

void MeleeUnitFSM::Update(double deltaTime)
{
    switch (GetCurrentState<STATES>())
    {
        case STATES::MOVING:
            MovingState(deltaTime);
            break;
        case STATES::ATTACKING:
            AttackingState();
            break;
        case STATES::IDLE:
            IdleState();
            break;
    }
}

void MeleeUnitFSM::MovingState(double deltaTime)
{
    auto direction = _target->getPosition()-GetOwner()->getPosition();
    //SceneA1::physicsEventQueue.push({GetOwner(),SceneA1::PhysEventType::MOVE,direction.Normalized()*_moveSpeed});
    GetOwner()->setPosition(GetOwner()->getPosition()+(direction*_moveSpeed));
}

void MeleeUnitFSM::AttackingState()
{

}

void MeleeUnitFSM::IdleState()
{
    _target = SceneA1::BruteForceFindCloseEntities(GetOppositeTeam(),GetOwner()->getPosition());

    if (_target)
        ChangeState<STATES>(STATES::MOVING);
}
