#include "message.hpp"

#include "SceneA1TakeTwo.hpp"
#include "FSMs/birdai.hpp"
#include "messager.hpp"

#include <algorithm>

void MoveEntityMessage::Handle()
{
	// Cast Sender to Entity | Non-Entities should not send this message
	Entity* ent = static_cast<Entity*>(entity);
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(ent,static_cast<Entity*>(target)->getPosition(),speed);
}

MoveEntityMessage::MoveEntityMessage(void* e, void* t, float s)
{
	entity = e;
	target = t;
	speed = s;
}

void RenderInfoTextMessage::Handle()
{
	SceneA1TakeTwo::texts.push_back({text,{x,y},size});
}

RenderInfoTextMessage::RenderInfoTextMessage(const std::string& t, float nx, float ny, float s)
{
	text = t;
	x = nx;
	y = ny;
	size = s;
}

KillAIMessage::KillAIMessage(void* sm)
{
	statemachine = sm;
}

void KillAIMessage::Handle()
{
	// Fetch AI Vector
	auto& ais = SceneA1TakeTwo::sms;
	// Find AI in Data Structure
	auto iter = std::find(ais.begin(),ais.end(),statemachine);
	// Delete Entity Itself
	SceneA1TakeTwo::AllGrid.despawnEntity((*iter)->getOwner());
	// Delete State Machine Object
	delete (*iter);
	ais.erase(iter);
}

void BirdRequestPreyMessage::Handle()
{
	auto find = SceneA1TakeTwo::AllGrid.findClosestEntity(static_cast<Entity*>(entity),"Prey",30);
	Messager::GetInstance().SendMessage("birds",std::make_shared<BirdPreyRequestReplyMessage>((void*)find,entity));
}

BirdRequestPreyMessage::BirdRequestPreyMessage(void* ent)
{
	entity = ent;
}

void BirdPreyRequestReplyMessage::Handle()
{
	static_cast<BirdAI*>(ownerBird)->huntingState.prey = static_cast<Entity*>(prey);
}

BirdPreyRequestReplyMessage::BirdPreyRequestReplyMessage(void* p, void* owner)
{
	prey = p;
	ownerBird = owner;
}

void BunnyCheckForPopulationCountMessage::Handle()
{
	auto vec = SceneA1TakeTwo::AllGrid.getAllWithTag("bunnies");
    *popSize = vec.size();
	if (vec.size() > 2)
	{
		// Fetch AI Vector
		auto& ais = SceneA1TakeTwo::sms;
		// Find AI in Data Structure
		auto iter = std::find(ais.begin(),ais.end(),statemachine);
		// Delete Entity Itself
		SceneA1TakeTwo::AllGrid.despawnEntity((*iter)->getOwner());
		// Delete State Machine Object
		delete (*iter);
		ais.erase(iter);
	}
}

BunnyCheckForPopulationCountMessage::BunnyCheckForPopulationCountMessage(void* ent, void* sm)
{
	owner = ent;
	statemachine = sm;
}

void MoveEntityUsingVectorMessage::Handle()
{
	SceneA1TakeTwo::AllGrid.moveEntityAlongGrid(static_cast<Entity*>(ent),{x,y,2},speed);
}

MoveEntityUsingVectorMessage::MoveEntityUsingVectorMessage(void* e, float s, float nx, float ny)
{
	ent = e;
	speed = s;
	x = nx;
	y = ny;
}

void BunnyFindMateMessage::Handle()
{
    (*mate) = SceneA1TakeTwo::AllGrid.findClosestEntity(static_cast<Entity*>(ent),"bunnies",30);
}

BunnyFindMateMessage::BunnyFindMateMessage(void* entity)
{
    ent = entity;
}
