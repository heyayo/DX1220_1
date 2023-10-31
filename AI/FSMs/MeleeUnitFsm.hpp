#ifndef DX1220_MELEEUNITFSM_HPP
#define DX1220_MELEEUNITFSM_HPP

#include "Fsm.hpp"

class MeleeUnitFSM : public FSM
{
    enum class STATES
    {
        MOVING,
        ATTACKING,
        IDLE,
        ALL_STATE
    };

    void MovingState(double deltaTime);
    void AttackingState();
    void IdleState();


public:
    MeleeUnitFSM(Entity* o, std::vector<Entity*>& opTeam);

    virtual void Update(double deltaTime) override;
};


#endif
