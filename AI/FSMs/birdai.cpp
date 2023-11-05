#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),5*deltaTime);
}

void MigrationState::Enter()
{
	// Search Whole Map for a Tree
	tree = SceneA1TakeTwo::AllGrid.findClosestEntity(state_machine->getOwner(),"Tree",30);
}

void MigrationState::Exit()
{

}
