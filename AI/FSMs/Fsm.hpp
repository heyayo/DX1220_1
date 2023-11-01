#ifndef DX1220_FSM_HPP
#define DX1220_FSM_HPP

#include "Entities/entity.hpp"
#include <vector>

struct STATE
{
    virtual void Update(double deltaTime) = 0;
    virtual STATE* StateChanger() = 0;
};

class FSM
{
    STATE* _state;
    Entity* owner;

public:
    FSM(Entity* o);
    virtual ~FSM() = default;

    constexpr void ChangeState(STATE* state)
    { _state = state; }
    STATE* GetCurrentState()
    { return _state; }

    Entity* GetOwner();

    virtual void Update(double deltaTime) = 0;
};

#endif
