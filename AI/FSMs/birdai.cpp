#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"
#include "logging.hpp"
#include "Application.h"

#include <random>

BirdAI::BirdAI(Entity* o, const std::vector<Entity*>& trees) : StateMachine(o), _trees(trees)
{
    // Randomize a tree to migrate to at first;
    std::uniform_int_distribution<int> rando(0,trees.size()-1);
    int tree_number = rando(Application::randomthing);
    _currentState = &migrationState;
	migrationState.tree = trees[tree_number];
}

Entity* BirdAI::getRandomTree(Entity* notThisOne)
{
	// Get Random Tree That isnt the current one
	int tree_number = 0;
	do
	{
		std::uniform_int_distribution<int> rando(0,_trees.size()-1);
		tree_number = rando(Application::randomthing);
	} while (_trees[tree_number] == notThisOne);
	return _trees[tree_number];
}

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),50*deltaTime);
	float diff = (tree->getPosition() - state_machine->getOwner()->getPosition()).LengthSquared();
	if ( 2 * 2 > diff )
	{
		tree = static_cast<BirdAI*>(state_machine)->getRandomTree(tree);
		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->perchState);
	}
}

void MigrationState::Enter()
{
	tree = static_cast<BirdAI*>(state_machine)->getRandomTree(tree);
    LOGINFO("Bird Begin Migration | " << state_machine->getOwner());
}

void MigrationState::Exit()
{
    LOGINFO("Bird End Migration | " << state_machine->getOwner());
}

PerchState::PerchState(StateMachine* stateMachine) : State(stateMachine) {}

void PerchState::Update(double deltaTime)
{

}

void PerchState::Enter()
{
    LOGINFO("Bird Perched | " << state_machine->getOwner());
	state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->migrationState);
}

void PerchState::Exit()
{
    LOGINFO("Bird Left Tree | " << state_machine->getOwner());
}
