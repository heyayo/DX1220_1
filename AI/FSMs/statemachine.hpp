#ifndef DX1220_STATEMACHINE_HPP
#define DX1220_STATEMACHINE_HPP

#include "Entities/entity.hpp"

struct State;

class StateMachine
{
protected:
	Entity* _owner;
	State* _currentState;
	
public:
	StateMachine(Entity* o);
	virtual ~StateMachine() = default;
	
	virtual void Update(double deltaTime);
	void ChangeState(State* nextState);
	virtual void RenderTexts();
	
	constexpr State* getCurrentState() { return _currentState; }
	constexpr Entity* getOwner() { return _owner; }
};


#endif
