#include "bunnyai.hpp"

// #include "messager.hpp"
// #include "message.hpp"
#include "Application.h"
#include "SceneA1TakeTwo.hpp"
#include "bulletboard.hpp"

#include "logging.hpp"

int BunnyAI::BunnyCount = 0;

BunnyAI::BunnyAI(Entity* o) : StateMachine(o)
{
	_currentState = &wander;
	// Messager::GetInstance().Register("bunnies",this);
	
	// Seed Wander State Direction to Prevent Uninitialized Number Wandering
	auto randomPosition = SceneA1TakeTwo::GetRandomLocationOnMap();
	wander.xTarget = randomPosition.first;
	wander.yTarget = randomPosition.second;

    ++BunnyCount;
}

BunnyAI::~BunnyAI()
{
    --BunnyCount;
}

void BunnyAI::Update(double deltaTime)
{
	StateMachine::Update(deltaTime); // This Piece is necessary as it calls Update on _currentState;
	mating_timer += deltaTime;
}

void SelfPopulationControlByImplodingState::Update(double deltaTime)
{
	timer += deltaTime;
	if (timer > 5)
	{
		state_machine->ChangeState(
			&static_cast<BunnyAI*>(state_machine)->wander);
	}
}

void SelfPopulationControlByImplodingState::Enter()
{
	LOGINFO("Bunny Deciding Whether Or Not To Self-Implode | " << state_machine->getOwner());
	timer = 0.f;
    auto iter = BulletBoard::GetInstance().Bird_Prey_Reservation.posts.find(state_machine->getOwner());
    if (iter == BulletBoard::GetInstance().Bird_Prey_Reservation.posts.end())
    {
        state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
        return;
    }
	auto vec = SceneA1TakeTwo::AllGrid.getAllWithTag("bunnies");
	if (vec.size() > 20)
	{
		// Bullet Board Death Queue
		BulletBoard::GetInstance().AI_Death_Queue.posts.push_back(state_machine);
//		// Fetch AI Vector
//		auto& ais = SceneA1TakeTwo::sms;
//		// Find AI in Data Structure
//		auto iter = std::find(ais.begin(),ais.end(),state_machine);
//		// Delete Entity Itself
//		SceneA1TakeTwo::AllGrid.despawnEntity((*iter)->getOwner());
//		// Delete State Machine Object
//		delete (*iter);
//		ais.erase(iter);
	}
	else
	{
		if (static_cast<BunnyAI*>(state_machine)->mating_timer > BUNNY_MATING_FREQUENCY)
		{
			state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->breeding);
			static_cast<BunnyAI*>(state_machine)->mating_timer = 0;
		}
		else
			state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
	}
//	Messager::GetInstance().SendMessage("scene",
//	std::make_shared<BunnyCheckForPopulationCountMessage>
//	    (state_machine->getOwner(),state_machine));
}

void SelfPopulationControlByImplodingState::Exit()
{
	LOGINFO("Bunny Stopped Deciding Whether Or Not To Self-Implode | " << state_machine->getOwner());
}

SelfPopulationControlByImplodingState::SelfPopulationControlByImplodingState(StateMachine* stateMachine) : State(
	stateMachine) {}

void BunnyWander::Update(double deltaTime)
{
//	Messager::GetInstance().SendMessage("scene",std::make_shared<MoveEntityUsingVectorMessage>(state_machine->getOwner(),10*deltaTime,x,y));
	auto myPos = state_machine->getOwner()->getPosition();
	Vector3 target{xTarget,yTarget,myPos.z};
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),target,30*deltaTime);
	if ((target - myPos).LengthSquared() < (BUNNY_MATE_PROXIMITY))
	{
		LOGINFO("Bunny Wander Target | " << target.x << '|' << target.y);
		LOGINFO("Bunny Position | " << myPos.x << '|' << myPos.y);
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->selfImplode);
	}
}

void BunnyWander::Enter()
{
	LOGINFO("Bunny Wandering | " << state_machine->getOwner());
	// Re-Randomize Wander Target Coordinates
	auto randomPosition = SceneA1TakeTwo::GetRandomLocationOnMap();
	xTarget = randomPosition.first;
	yTarget = randomPosition.second;
}

void BunnyWander::Exit()
{
	LOGINFO("Bunny Stopped Wandering | " << state_machine->getOwner());
}

BunnyWander::BunnyWander(StateMachine* stateMachine) : State(stateMachine) {}

void BunnyBreed::Update(double deltaTime)
{
	// TODO MOVEMENT SPEED BALANCING
	if (!mate)
	{
		// If Bunny cannot find love, seek happiness elsewhere
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
	}
	
	// Move towards Mate
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),mate->getPosition(),50*deltaTime);
	auto diff = mate->getPosition() - state_machine->getOwner()->getPosition();
	if (diff.LengthSquared() < (BUNNY_MATE_PROXIMITY))
	{
		// Once reached, start family
		auto child = SceneA1TakeTwo::SpawnAI<BunnyAI>();
		child->setPosition(state_machine->getOwner()->getPosition());
		child->setTag("bunnies");
		child->setScale({20,20,20});
		
		// Make both bunnies leave for milk
		BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.erase({state_machine->getOwner(),mate});
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
	}
	// Old Messager System to Move Bunny
//    Messager::GetInstance().SendMessageInstant("scene",std::make_shared<MoveEntityMessage>(state_machine->getOwner(),mate,60*deltaTime));
}

void BunnyBreed::Enter()
{
	LOGINFO("Bunny Searching for a Mate | " << state_machine->getOwner());
    // Message will Find Bunny Mate
    // Messager::GetInstance().SendMessageInstant("scene",std::make_shared<BunnyFindMateMessage>(static_cast<Entity*>(state_machine->getOwner())));
	
	// Check if there is a mate coming
	auto iter = std::find_if(BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.begin(),
							 BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.end(),
							 [&](const auto& val){ return val.second == state_machine->getOwner(); });
	if (iter != BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.end())
	{
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->awaitMate);
		static_cast<BunnyAI*>(state_machine)->awaitMate.mate = (*iter).second;
		LOGINFO("Bunny is awaiting a mate | " << state_machine->getOwner());
		return;
	}
	// Find a mate throughout the entire map, ignore taken candidates and those marked for death by birds
	std::set<Entity*> all_bunnies_no_duplicates;
	for (auto post : BulletBoard::GetInstance().Bunny_Mate_Reservation.posts)
	{
		all_bunnies_no_duplicates.insert(post.first);
		all_bunnies_no_duplicates.insert(post.second);
	}
	for (auto post : BulletBoard::GetInstance().Bird_Prey_Reservation.posts)
	{
		all_bunnies_no_duplicates.insert(post);
	}
	std::vector<Entity*> all_bunnies(all_bunnies_no_duplicates.size());
	std::copy(
		all_bunnies_no_duplicates.begin(),
		all_bunnies_no_duplicates.end(),
		std::back_inserter(all_bunnies)
		);
	mate = SceneA1TakeTwo::AllGrid.findClosestEntity(state_machine->getOwner(),"bunnies",all_bunnies,30);
	if (!mate)
	{
		LOGINFO("There are no available mates for this bunny | " << state_machine->getOwner());
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
		return;
	}
	
	// Mark both bunnies as Taken
	BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.insert({state_machine->getOwner(),mate});
}

void BunnyBreed::Exit()
{
    LOGINFO("Bunny Mating Season Ended | " << state_machine->getOwner());
}

BunnyBreed::BunnyBreed(StateMachine* stateMachine) : State(stateMachine) {}

void BunnyAwaitMate::Update(double deltaTime)
{
	if (!BulletBoard::GetInstance().Bunny_Mate_Reservation.posts.count({ state_machine->getOwner(),mate}))
		state_machine->ChangeState(&static_cast<BunnyAI*>(state_machine)->wander);
}

void BunnyAwaitMate::Enter()
{
	LOGINFO("Bunny is waiting for its mate | " << state_machine->getOwner());
}

void BunnyAwaitMate::Exit()
{
	LOGINFO("Bunny has mated | " << state_machine->getOwner());
}

BunnyAwaitMate::BunnyAwaitMate(StateMachine* stateMachine) : State(stateMachine) {}
