#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"
#include "logging.hpp"
#include "Application.h"

#include <random>

BirdAI::BirdAI(Entity* o, const std::vector<Entity*>& trees) : StateMachine(o), _trees(trees)
{
    // Randomize a tree to migrate to at first;
    std::uniform_int_distribution<int> rando(trees.size());
    int tree_number = rando(Application::randomthing);
    _currentState = &_migState;
    _migState.tree = trees[tree_number];
}

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),50*deltaTime);
}

void MigrationState::Enter()
{
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
}

void PerchState::Exit()
{
    LOGINFO("Bird Left Tree | " << state_machine->getOwner());
}
