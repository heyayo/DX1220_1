#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"
#include "logging.hpp"
#include "Application.h"
#include "messager.hpp"

#include <random>
#include <sstream>

BirdAI::BirdAI(Entity* o, const std::vector<Entity*>& trees) : StateMachine(o), _trees(trees)
{
    // Randomize a tree to migrate to at first;
    std::uniform_int_distribution<int> rando(0,trees.size()-1);
    int tree_number = rando(Application::randomthing);
    _currentState = &migrationState;
	migrationState.tree = trees[tree_number];
	Messager::GetInstance().Register("birds"); // Same Address for All Birds | Change if needed
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

void BirdAI::RenderTexts()
{
	std::ostringstream ss;
	ss.precision(5);
	ss << "Hunger: " << hunger;
	auto pos = getOwner()->getPosition();
	Messager::GetInstance().SendMessage("scene",std::make_shared<RenderInfoTextMessage>(ss.str().c_str(),pos.x,pos.y,30));
}

void BirdAI::TickHunger(double dt, double multiplier)
{
	hunger -= dt * multiplier;
}

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	// Direct Scene Access
	// SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),50*deltaTime);
	
	// Messager System Scene Access
	Messager::GetInstance().SendMessage("scene",std::shared_ptr<msg_base>(new MoveEntityMessage(state_machine->getOwner(),tree,50*deltaTime)));
	
	float diff = (tree->getPosition() - state_machine->getOwner()->getPosition()).LengthSquared();
	if ( 2 * 2 > diff )
	{
		tree = static_cast<BirdAI*>(state_machine)->getRandomTree(tree);
		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->perchState);
	}
	static_cast<BirdAI*>(state_machine)->TickHunger(deltaTime);
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
	static double timer = 0.f;
	static_cast<BirdAI*>(state_machine)->TickHunger(deltaTime);
	timer += deltaTime;
	if (timer > 3)
	{
		// Bird Ends Perch
		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->migrationState);
		timer = 0.f;
	}
}

void PerchState::Enter()
{
    LOGINFO("Bird Perched | " << state_machine->getOwner());
}

void PerchState::Exit()
{
    LOGINFO("Bird Left Tree | " << state_machine->getOwner());
}
