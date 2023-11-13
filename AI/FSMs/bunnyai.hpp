#ifndef DX1220_BUNNYAI_HPP
#define DX1220_BUNNYAI_HPP

#include "statemachine.hpp"
#include "state.hpp"

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
	float x,y;
	float xTarget, yTarget;
	
	void Update(double deltaTime) override;
	void Enter() override;
	void Exit() override;
	
	BunnyWander(StateMachine* stateMachine);
};

class BunnyAI : public StateMachine
{

public:
	BunnyAI(Entity* o);
	
	SelfPopulationControlByImplodingState selfImplode{this};
	BunnyWander wander{this};
	
};


#endif
