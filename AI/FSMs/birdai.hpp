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

class BirdAI : public StateMachine
{
    std::vector<Entity*> _trees;

public:
    BirdAI(Entity* o, const std::vector<Entity*>& trees);
	
    MigrationState migrationState{this};
    PerchState     perchState{this};
	
	Entity* getRandomTree(Entity* notThisOne);
};

#endif
