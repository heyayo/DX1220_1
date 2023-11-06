#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"

BirdAI::BirdAI(Entity* o) : StateMachine(o)
{
    _currentState = &_migState;
    _trees = SceneA1TakeTwo::AllGrid.getAllWithTag("Tree");
}

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),50*deltaTime);
}

void MigrationState::Enter()
{
	// Search Whole Map for a Tree
	tree = SceneA1TakeTwo::AllGrid.findClosestEntity(state_machine->getOwner(),"Tree",30);
}

void MigrationState::Exit()
{

}

PerchState::PerchState(StateMachine* stateMachine) : State(stateMachine) {}

void PerchState::Update(double deltaTime)
{

}

void PerchState::Enter()
{

}

void PerchState::Exit()
{

}
