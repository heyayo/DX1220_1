#ifndef DX1220_BIRDAI_HPP
#define DX1220_BIRDAI_HPP

#include "statemachine.hpp"
#include "state.hpp"
#include "Entities/entity.hpp"

struct MigrationState : public State
{
	MigrationState(StateMachine* stateMachine);
	
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	Entity* tree;
};

class BirdAI : public StateMachine
{
};

#endif
