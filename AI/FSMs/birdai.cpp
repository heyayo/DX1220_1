#include "birdai.hpp"

#include "SceneA1TakeTwo.hpp"
#include "logging.hpp"
#include "Application.h"
// #include "messager.hpp"
#include "bulletboard.hpp"

#include <random>
#include <sstream>

BirdAI::BirdAI(Entity* o, const std::vector<Entity*>& trees) : StateMachine(o), _trees(trees)
{
    // Randomize a tree to migrate to at first;
    std::uniform_int_distribution<int> rando(0,trees.size()-1);
    int tree_number = rando(Application::randomthing);
    _currentState = &migrationState;
	migrationState.tree = trees[tree_number];
	// Messager::GetInstance().Register("birds", nullptr); // Same Address for All Birds | Change if needed
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
	SceneA1TakeTwo::texts.push_back({ss.str(),{pos.x,pos.y},30});
//	Messager::GetInstance().SendMessage("scene",std::make_shared<RenderInfoTextMessage>(ss.str().c_str(),pos.x,pos.y,30));
}

void BirdAI::TickHunger(double dt, double multiplier)
{
	hunger -= dt * multiplier;
}

void BirdAI::Update(double deltaTime)
{
	StateMachine::Update(deltaTime);
	if (hunger <= 0)
	{
		// Old Messager Death Queue
		// Messager::GetInstance().SendMessage("scene",std::make_shared<KillAIMessage>(this));
		BulletBoard::GetInstance().AI_Death_Queue.posts.push_back(this);
	}
	TickHunger(deltaTime); // Constant Hunger Ticking
}

float BirdAI::getHunger()
{
	return hunger;
}

void BirdAI::addHunger(float value)
{
	hunger += value;
}

float BirdAI::getMoveSpeed()
{
	// More Movespeed when hungrier
	return 50.0f + (100.0f - hunger);
}

MigrationState::MigrationState(StateMachine* stateMachine) : State(stateMachine) {}

void MigrationState::Update(double deltaTime)
{
	// Direct Scene Access
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),tree->getPosition(),static_cast<BirdAI*>(state_machine)->getMoveSpeed()*deltaTime);
	
	// Messager System Scene Access
//	Messager::GetInstance().SendMessage("scene",
//	std::shared_ptr<msg_base>(
//		new MoveEntityMessage(state_machine->getOwner(),
//		  tree,
//		  static_cast<BirdAI*>(state_machine)->moveSpeed*deltaTime)));
	
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
	static double timer = 0.f;
	timer += deltaTime;
	if (static_cast<BirdAI*>(state_machine)->getHunger() < 60)
	{
		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->huntingState);
		timer = 0.f;
		return;
	}
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

HuntingState::HuntingState(StateMachine* stateMachine) : State(stateMachine)
{

}

void HuntingState::Update(double deltaTime)
{
	// Timer Unnecessary now
//	static double timer = 0.f;
//
//	timer += deltaTime;
//
//	if (timer > 1) // If Bird is unable to find a prey
//	{
//		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->migrationState);
//		timer = 0.f;
//	}
	
	if (prey)
	{
		SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),prey->getPosition(),static_cast<BirdAI*>(state_machine)->getMoveSpeed()*deltaTime);
//		Messager::GetInstance().SendMessage("scene",
//											std::make_shared<MoveEntityMessage>(state_machine->getOwner(),prey,
//												static_cast<BirdAI*>(state_machine)->moveSpeed*deltaTime));
		auto diff = prey->getPosition() - state_machine->getOwner()->getPosition();
		if (diff.LengthSquared() < (10))
		{
			// Prey will always have a StateMachine attached to it, if it doesn't then the problem lies elsewhere
			BulletBoard::GetInstance().AI_Death_Queue.posts.push_back(SceneA1TakeTwo::GetSMFromEntity(prey));
			// Prey is dead, remove from reservation as it is invalid now
			BulletBoard::GetInstance().Bird_Prey_Reservation.posts.erase(prey);
			// BUNNY FOOD VALUE
			static_cast<BirdAI*>(state_machine)->addHunger(30);
			// null prey ptr incase
			prey = nullptr;
		}
	}
	else
	{
		state_machine->ChangeState(&static_cast<BirdAI*>(state_machine)->migrationState);
		LOGINFO("Bird did not find a suitable prey | " << state_machine->getOwner());
	}
}

void HuntingState::Enter()
{
	LOGINFO("Bird Hunting for Food | " << state_machine->getOwner());
	prey = SceneA1TakeTwo::AllGrid.findClosestEntity(state_machine->getOwner(),"bunnies",30); // Add Exceptions for Tracked Bunnies
	BulletBoard::GetInstance().Bird_Prey_Reservation.posts.insert(prey);
//	Messager::GetInstance().SendMessage("scene",std::make_shared<BirdRequestPreyMessage>(state_machine->getOwner()));
}

void HuntingState::Exit()
{
	LOGINFO("Bird Ended Hunt | " << state_machine->getOwner());
}
