#ifndef DX1220_MESSAGE_HPP
#define DX1220_MESSAGE_HPP

#include <string>

struct msg_base
{
	virtual void Handle() = 0;
};

struct MoveEntityMessage : public msg_base
{
	void* entity;
	void* target;
	float speed;
	
	void Handle() override;
	
	MoveEntityMessage(void* e, void* t, float s);
};

struct RenderInfoTextMessage : public msg_base
{
	std::string text;
	float x,y;
	float size = 10.f;
	
	void Handle() override;
	
	RenderInfoTextMessage(const std::string& t, float nx, float ny, float s);
};


#endif
