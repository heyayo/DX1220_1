#include "bunnyai.hpp"

#include "messager.hpp"
#include "message.hpp"
#include "Application.h"

#include "logging.hpp"

BunnyAI::BunnyAI(Entity* o) : StateMachine(o)
{
	_currentState = &wander;
	Messager::GetInstance().Register("bunnies",this);
	
	std::uniform_real_distribution<float> rando(-50,50);
	wander.x = rando(Application::randomthing);
	wander.y = rando(Application::randomthing);
	auto pos = getOwner()->getPosition();
	wander.xTarget = pos.x + wander.x;
	wander.yTarget = pos.y + wander.y;
}

void SelfPopulationControlByImplodingState::Update(double deltaTime)
{
	timer += deltaTime;
    if (static_cast<BunnyAI*>(state_machine)->popSizeTracker < 20)
    {
        state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->breeding);
        return;
    }
	if (timer > 5)
	{
		state_machine->ChangeState(
			&static_cast<BunnyAI*>(state_machine)->wander);
	}
}

void SelfPopulationControlByImplodingState::Enter()
{
	timer = 0.f;
	Messager::GetInstance().SendMessage("scene",
	std::make_shared<BunnyCheckForPopulationCountMessage>
	    (state_machine->getOwner(),state_machine));
	LOGINFO("Bunny Deciding Whether Or Not To Self-Implode | " << state_machine->getOwner());
}

void SelfPopulationControlByImplodingState::Exit()
{
	LOGINFO("Bunny Stopped Deciding Whether Or Not To Self-Implode | " << state_machine->getOwner());
}

SelfPopulationControlByImplodingState::SelfPopulationControlByImplodingState(StateMachine* stateMachine) : State(
	stateMachine) {}

void BunnyWander::Update(double deltaTime)
{
	Messager::GetInstance().SendMessage("scene",std::make_shared<MoveEntityUsingVectorMessage>(state_machine->getOwner(),10*deltaTime,x,y));
	auto myPos = state_machine->getOwner()->getPosition();
	Vector3 target{xTarget,yTarget,myPos.z};
	if ((target - myPos).LengthSquared() < (25))
	{
		LOGINFO("Bunny Wander Target | " << target.x << '|' << target.y);
		LOGINFO("Bunny Position | " << myPos.x << '|' << myPos.y);
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->selfImplode);
	}
}

void BunnyWander::Enter()
{
	LOGINFO("Bunny Wandering | " << state_machine->getOwner());
	std::uniform_real_distribution<float> rando(-50,50);
	x = rando(Application::randomthing);
	y = rando(Application::randomthing);
	auto pos = state_machine->getOwner()->getPosition();
	xTarget = pos.x + x;
	yTarget = pos.y + y;
}

void BunnyWander::Exit()
{
	LOGINFO("Bunny Stopped Wandering | " << state_machine->getOwner());
}

BunnyWander::BunnyWander(StateMachine* stateMachine) : State(stateMachine) {}

void BunnyBreed::Update(double deltaTime)
{
    Messager::GetInstance().SendMessageInstant("scene",std::make_shared<MoveEntityMessage>(state_machine->getOwner(),mate,60*deltaTime));
}

void BunnyBreed::Enter()
{
    LOGINFO("Bunny Searching for a Mate | " << state_machine->getOwner());
    // Message will Find Bunny Mate
    Messager::GetInstance().SendMessageInstant("scene",std::make_shared<BunnyFindMateMessage>(static_cast<Entity*>(state_machine->getOwner())));
}

void BunnyBreed::Exit()
{
    LOGINFO("Bunny Mating Season Ended | " << state_machine->getOwner());
}

BunnyBreed::BunnyBreed(StateMachine* stateMachine) : State(stateMachine) {}
