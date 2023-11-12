#include "message.hpp"

#include "SceneA1TakeTwo.hpp"

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
