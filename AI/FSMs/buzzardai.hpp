#ifndef DX1220_BUZZARDAI_HPP
#define DX1220_BUZZARDAI_HPP

#include "statemachine.hpp"
#include "state.hpp"

#define BUZZARD_HUNGER_RATE 2
#define BUZZARD_LIMIT 3
#define BUZZARD_SPEED 200
#define BUZZARD_CIRCLE_RADIUS 150

struct WatchPopulations : public State
{
    float x;
    float y;

    WatchPopulations(StateMachine* stateMachine);
    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

struct HuntPrey : public State
{
    HuntPrey(StateMachine* stateMachine);
    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

struct CirclePrey : public State
{
    float circleEngine = 0.f;
    float timer = 0.f;

    CirclePrey(StateMachine* stateMachine);
    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

struct RunToCirclePrey : public State
{
    RunToCirclePrey(StateMachine* stateMachine);
    virtual void Update(double deltaTime) override;
    virtual void Enter() override;
    virtual void Exit() override;
};

class BuzzardAI : public StateMachine
{
public:
    static int BuzzardCount;

    BuzzardAI(Entity* o);
    ~BuzzardAI();

    void Update(double deltaTime) override;

    double timer = 0.0;
    float hunger = 100.f;
    Entity* prey = nullptr;

    WatchPopulations watchPop{this};
    HuntPrey hunting{this};
    CirclePrey circle{this};
    RunToCirclePrey runTo{this};
};


#endif
