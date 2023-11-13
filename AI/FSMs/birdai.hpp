#ifndef DX1220_BIRDAI_HPP
#define DX1220_BIRDAI_HPP

#include "statemachine.hpp"
#include "state.hpp"
#include "Entities/entity.hpp"

#include <vector>

struct MigrationState : public State
{
	MigrationState(StateMachine* stateMachine);
	
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	Entity* tree;
};

struct PerchState : public State
{
    PerchState(StateMachine* stateMachine);

    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

struct HuntingState : public State
{
	HuntingState(StateMachine* stateMachine);
	
	virtual void Update(double deltaTime) override;
	virtual void Enter() override;
	virtual void Exit() override;
	
	Entity* prey = nullptr;
};

class BirdAI : public StateMachine
{
    std::vector<Entity*> _trees;
	float hunger = 100.f;

public:
    BirdAI(Entity* o, const std::vector<Entity*>& trees);
	
	void Update(double deltaTime);
	void RenderTexts();
	
    MigrationState migrationState{this};
    PerchState     perchState{this};
	HuntingState   huntingState{this};
	float moveSpeed = 50.f;
	
	Entity* getRandomTree(Entity* notThisOne);
	float getHunger();
	void TickHunger(double dt, double multiplier = 3.0);
};

#endif
