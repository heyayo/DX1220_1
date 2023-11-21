#include "beeai.hpp"
#include "bulletboard.hpp"
#include "SceneA1TakeTwo.hpp"
#include "Application.h"
#include "logging.hpp"

#include <random>

// TODO TEST BEE AI

HiveData BeeAI::hiveData;

BeeAI::BeeAI(Entity* o, const std::vector<Entity*>& trees, Entity* beehive) : StateMachine(o)
{
	treeList = trees;
	hive     = beehive;
	
	std::uniform_int_distribution<int> rando(0,3);
	treeTarget = rando(Application::randomthing);
	
	getOwner()->setTag("bees");
	getOwner()->setScale({10, 10, 10});
	
	++hiveData.beeCount;

    _currentState = &harvestPollen;
	
	// Varying Lifespan
	std::uniform_real_distribution<float> randofloat(-10,10);
	lifeSpan = BEE_LIFETIME + randofloat(Application::randomthing);
}

BeeAI::~BeeAI()
{
	--hiveData.beeCount;
}

void BeeAI::Update(double deltaTime)
{
	StateMachine::Update(deltaTime);
	lifeTime += deltaTime;
	
	if (lifeTime > lifeSpan)
	{
		// Kill Self when old
		BulletBoard::GetInstance().AI_Death_Queue.posts.push_back(this);
	}
}

void BeeHarvestPollen::Update(double deltaTime)
{
	auto beeStateMachine = static_cast<BeeAI*>(state_machine);
	
	// Move to tree
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),
												beeStateMachine->treeList[beeStateMachine->treeTarget]->getPosition(),
												BEE_MOVE_SPEED*deltaTime);
	
	// Tree Detection (Harvest Pollen on touch)
	auto diff = beeStateMachine->treeList[beeStateMachine->treeTarget]->getPosition() - state_machine->getOwner()->getPosition();
	if (diff.LengthSquared() < (20))
	{
		beeStateMachine->localPollenStorage += 15.f;
		beeStateMachine->treeTarget = (beeStateMachine->treeTarget + 1) % 4;
	}

    if (BeeAI::hiveData.pollenCount > 125)
    {
        state_machine->ChangeState(&beeStateMachine->respondToHive);
        return;
    }
	
	// Move back to Hive when holding enough Pollen
	if (beeStateMachine->localPollenStorage > 50.f)
		state_machine->ChangeState(&beeStateMachine->depositPollen);
}

void BeeHarvestPollen::Enter()
{
	LOGINFO("Bee Harvesting Pollen | " << state_machine->getOwner());
}

void BeeHarvestPollen::Exit()
{
	LOGINFO("Bee Harvested Pollen | " << state_machine->getOwner());
}

BeeHarvestPollen::BeeHarvestPollen(StateMachine* stateMachine) : State(stateMachine) {}

void BeeDepositPollen::Update(double deltaTime)
{
	auto beeSM = static_cast<BeeAI*>(state_machine);
    if (BeeAI::hiveData.pollenCount > 125)
    { // TODO BulletBoard to prevent bees helping too much
        state_machine->ChangeState(&beeSM->respondToHive);
        return;
    }
    SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),beeSM->hive->getPosition(),BEE_MOVE_SPEED*deltaTime);
	auto diff = beeSM->hive->getPosition() - state_machine->getOwner()->getPosition();
	if (diff.LengthSquared() < (20))
	{
		// Deposit Pollen in Hive
		BeeAI::hiveData.pollenCount += beeSM->localPollenStorage;
		beeSM->localPollenStorage = 0;
		state_machine->ChangeState(&beeSM->harvestPollen);
	}
}

void BeeDepositPollen::Enter()
{
	LOGINFO("Bee Depositing Pollen | " << state_machine->getOwner());
}

void BeeDepositPollen::Exit()
{
	LOGINFO("Bee Deposited Pollen | " << state_machine->getOwner());
}

BeeDepositPollen::BeeDepositPollen(StateMachine* stateMachine) : State(stateMachine) {}

void BeeRespondToHive::Update(double deltaTime)
{
	auto beeSM = static_cast<BeeAI*>(state_machine);

    SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),beeSM->hive->getPosition(),BEE_MOVE_SPEED*deltaTime);
	auto diff = beeSM->hive->getPosition() - state_machine->getOwner()->getPosition();
	
	if (BeeAI::hiveData.pollenCount < 10)
	{
        LOGINFO("HIVE LACKS POLLEN TO DO ANYTHING | " << state_machine->getOwner());
		state_machine->ChangeState(&beeSM->harvestPollen);
		return;
	}
	if (diff.LengthSquared() < (20))
	{
		if (BeeAI::hiveData.makeHoneyInsteadOfMakingBeesThreshold < BeeAI::hiveData.beeCount)
		{
			BeeAI::hiveData.pollenCount -= HONEY_COST;
			BeeAI::hiveData.honeyCount += HONEY_COST;
            LOGINFO("BEE MAKES HONEY | " << state_machine->getOwner());
		}
		else
		{
			BeeAI::hiveData.pollenCount -= BEE_COST;
			// Spawn Bee
			auto bee = SceneA1TakeTwo::SpawnAI<BeeAI>(beeSM->treeList,beeSM->hive);
            bee->setPosition(beeSM->getOwner()->getPosition());
            LOGINFO("A NEW BEE HAS BEEN BORN | " << state_machine->getOwner());
		}
		state_machine->ChangeState(&beeSM->harvestPollen);
	}
}

void BeeRespondToHive::Enter()
{
	LOGINFO("Bee Answering To Hive | " << state_machine->getOwner());
	std::uniform_int_distribution<int> rando(0,3);
	static_cast<BeeAI*>(state_machine)->treeTarget = rando(Application::randomthing);
}

void BeeRespondToHive::Exit()
{
	LOGINFO("Bee Answered Hive | " << state_machine->getOwner());
}

BeeRespondToHive::BeeRespondToHive(StateMachine* stateMachine) : State(stateMachine) {}
