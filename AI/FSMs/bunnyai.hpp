#ifndef DX1220_BUNNYAI_HPP
#define DX1220_BUNNYAI_HPP

#include "statemachine.hpp"
#include "state.hpp"

#define BUNNY_MATE_PROXIMITY 10
#define BUNNY_MATING_FREQUENCY 2
#define BUNNY_LIMITS 50

struct SelfPopulationControlByImplodingState : public State
{
	double timer = 0.f;
	
	virtual void Update(double deltaTime) override;
	virtual void Enter() override;
	virtual void Exit() override;
	
	SelfPopulationControlByImplodingState(StateMachine* stateMachine);
};

struct BunnyWander : public State
{
	float xTarget, yTarget;
	
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	BunnyWander(StateMachine* stateMachine);
};

struct BunnyBreed : public State
{
    Entity* mate = nullptr; // Closest Bunny

    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;

    BunnyBreed(StateMachine* stateMachine);
};

struct BunnyAwaitMate : public State
{
	Entity* mate = nullptr;
	
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	BunnyAwaitMate(StateMachine* stateMachine);
};

class BunnyAI : public StateMachine
{
public:
    static int BunnyCount;

	BunnyAI(Entity* o);
    ~BunnyAI();
	
	float mating_timer = 0.f;

	SelfPopulationControlByImplodingState selfImplode{this};
	BunnyWander wander{this};
	BunnyBreed breeding{this};
	BunnyAwaitMate awaitMate{this};
	
	void Update(double deltaTime) override;
};


#endif
