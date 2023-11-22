#include "buzzardai.hpp"

#include "SceneA1TakeTwo.hpp"
#include "bulletboard.hpp"
#include "logging.hpp"

#include "birdai.hpp"
#include "bunnyai.hpp"
#include "beeai.hpp"

#include <cmath>

int BuzzardAI::BuzzardCount = 0;

BuzzardAI::BuzzardAI(Entity* o) : StateMachine(o) {}

void BuzzardAI::Update(double deltaTime)
{
    StateMachine::Update(deltaTime);
    hunger -= deltaTime * BUZZARD_HUNGER_RATE;
    ++BuzzardCount;
}
BuzzardAI::~BuzzardAI() { --BuzzardCount; }

WatchPopulations::WatchPopulations(StateMachine* stateMachine) : State(stateMachine) {}

void WatchPopulations::Update(double deltaTime)
{
    BuzzardAI* sm = static_cast<BuzzardAI*>(state_machine);
    auto pos = sm->getOwner()->getPosition();
    Vector3 target{x,y,pos.z};
    SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(state_machine->getOwner(),target,BUZZARD_SPEED*deltaTime);
    auto diff = target - pos;
    if (diff.LengthSquared() < (10))
    {
        state_machine->ChangeState(&sm->circle);
    }
}

void WatchPopulations::Enter()
{
    LOGINFO("Buzzard Watching Populations | " << state_machine->getOwner());
    auto pos = SceneA1TakeTwo::GetRandomLocationOnMap();
    x = pos.first;
    y = pos.second;
}

void WatchPopulations::Exit()
{
    LOGINFO("Buzzard Finished Watching | " << state_machine->getOwner());
}

HuntPrey::HuntPrey(StateMachine* stateMachine) : State(stateMachine) {}

void HuntPrey::Update(double deltaTime)
{
    BuzzardAI* sm = static_cast<BuzzardAI*>(state_machine);
    SceneA1TakeTwo::AllGrid.moveEntityAlongGrid
    (state_machine->getOwner(),
     sm->prey->getPosition(),
     BUZZARD_SPEED*deltaTime);
    auto diff = sm->prey->getPosition() - state_machine->getOwner()->getPosition();
    if (diff.LengthSquared() < (25))
    {
        BulletBoard::GetInstance().AI_Death_Queue.posts.push_back(SceneA1TakeTwo::GetSMFromEntity(sm->prey));
        BulletBoard::GetInstance().Bird_Prey_Reservation.posts.erase(sm->prey);

        sm->prey = nullptr;
        state_machine->ChangeState(&sm->watchPop);
    }
}

void HuntPrey::Enter()
{
    LOGINFO("BUZZARD BEGIN HUNT | " << state_machine->getOwner());
}

void HuntPrey::Exit()
{
    LOGINFO("BUZZARD ENDING HUNT | " << state_machine->getOwner());
    static_cast<BuzzardAI*>(state_machine)->prey = nullptr;
}

CirclePrey::CirclePrey(StateMachine* stateMachine) : State(stateMachine)
{

}

void CirclePrey::Update(double deltaTime)
{
    auto sm = static_cast<BuzzardAI*>(state_machine);
    if (!sm->prey)
    {
        sm->ChangeState(&sm->watchPop);
        return;
    }
    circleEngine += deltaTime;

    float sin = std::sin(circleEngine) * BUZZARD_CIRCLE_RADIUS;
    float cos = std::cos(circleEngine) * BUZZARD_CIRCLE_RADIUS;
    auto target = sm->prey->getPosition();
    target.x += sin;
    target.y += cos;

    SceneA1TakeTwo::AllGrid.moveEntityAlongGrid
    (state_machine->getOwner(),
    target,
    BUZZARD_SPEED*deltaTime);

    if (circleEngine > 5)
    {
        state_machine->ChangeState(&sm->hunting);
    }
}

void CirclePrey::Enter()
{
    circleEngine = 0.f;
}

void CirclePrey::Exit()
{

}
