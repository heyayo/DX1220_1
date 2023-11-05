#ifndef DX1220_STATEMACHINE_HPP
#define DX1220_STATEMACHINE_HPP

#include "Entities/entity.hpp"

struct State;

class StateMachine
{
	Entity* _owner;
	State* _currentState;
	
public:
	StateMachine(Entity* o);
	
	void Update(double deltaTime);
	void ChangeState(State* nextState);
	
	constexpr State* getCurrentState() { return _currentState; }
	constexpr Entity* getOwner() { return _owner; }
};


#endif
