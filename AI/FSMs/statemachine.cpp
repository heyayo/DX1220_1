#include "statemachine.hpp"

#include "state.hpp"

StateMachine::StateMachine(Entity* o) : _owner(o) {}

void StateMachine::Update(double deltaTime)
{
	_currentState->Update(deltaTime);
}

void StateMachine::ChangeState(State* nextState)
{
	_currentState->Exit();
	_currentState = nextState;
	_currentState->Enter();
}
