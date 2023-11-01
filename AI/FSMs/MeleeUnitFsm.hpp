#ifndef DX1220_MELEEUNITFSM_HPP
#define DX1220_MELEEUNITFSM_HPP

#include "Fsm.hpp"

class MeleeUnitFSM : public FSM
{
    enum class STATES
    {
        MOVING,
        ATTACKING,
        SEARCHING
    };

public:
    MeleeUnitFSM(Entity* o);

    virtual void Update(double deltaTime) override;
};


#endif
