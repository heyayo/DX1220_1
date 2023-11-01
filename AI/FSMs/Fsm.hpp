#ifndef DX1220_FSM_HPP
#define DX1220_FSM_HPP

#include "Entities/entity.hpp"
#include <vector>

class FSM
{
    int _state = 0;
    Entity* owner;

protected:
    Entity* _target;
    float _moveSpeed;

public:
    FSM(Entity* o);
    virtual ~FSM() = default;

    template<typename T>
    void ChangeState(T state)
    { _state = static_cast<int>(state); }
    template<typename T>
    T GetCurrentState()
    { return static_cast<T>(_state); }

    Entity* GetOwner();

    virtual void Update(double deltaTime) = 0;
};

#endif
