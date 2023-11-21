#ifndef DX1220_BEEAI_HPP
#define DX1220_BEEAI_HPP

#include <vector>
#include "statemachine.hpp"
#include "state.hpp"

// Bees live for 30 seconds
#define BEE_LIFETIME 30
#define BEE_COST 25.f
#define HONEY_COST 10.f
#define BEE_MOVE_SPEED 250

struct HiveData
{
	float pollenCount = 0.f; // How much pollen there is
	float honeyCount = 0.f;
	int beeCount = 0;
	const int makeHoneyInsteadOfMakingBeesThreshold = 10; // Make Honey when there are 10 bees
};

struct BeeHarvestPollen : public State
{
	virtual void Update(double deltaTime) override;
	virtual void Enter() override;
	virtual void Exit() override;
	
	BeeHarvestPollen(StateMachine* stateMachine);
};

struct BeeDepositPollen : public State
{
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	BeeDepositPollen(StateMachine* stateMachine);
};

struct BeeRespondToHive : public State
{
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	BeeRespondToHive(StateMachine* stateMachine);
};

class BeeAI : public StateMachine
{
public:
	static HiveData hiveData;

	float localPollenStorage = 0.f;
	std::vector<Entity*> treeList;
	int                  treeTarget = 0;
	Entity* hive;
	float lifeTime = 0.f;

	BeeAI(Entity* o, const std::vector<Entity*>& trees, Entity* beehive);
	~BeeAI();
	
	void Update(double deltaTime) override;
	
	BeeHarvestPollen harvestPollen{this};
	BeeDepositPollen depositPollen{this};
	BeeRespondToHive respondToHive{this};
	
};


#endif
