#ifndef DX1220_STATE_HPP
#define DX1220_STATE_HPP

class StateMachine;

struct State
{
	StateMachine* state_machine;
	
	virtual void Update(double deltaTime) = 0;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	
	State(StateMachine* stateMachine);
};


#endif
