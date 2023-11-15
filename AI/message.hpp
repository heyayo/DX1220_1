#ifndef DX1220_MESSAGE_HPP
#define DX1220_MESSAGE_HPP

#include <string>
#include <vector>

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

struct KillAIMessage : public msg_base
{
	void* statemachine;
	
	void Handle() override;
	
	KillAIMessage(void* sm);
};

struct BirdRequestPreyMessage : public msg_base
{
	void* entity;
	
	void Handle() override;
	
	BirdRequestPreyMessage(void* ent);
};

struct BirdPreyRequestReplyMessage : public msg_base
{
	void* prey;
	void* ownerBird;
	
	void Handle() override;
	
	BirdPreyRequestReplyMessage(void* p, void* owner);
};

struct BunnyCheckForPopulationCountMessage : public msg_base
{
	void* owner;
	void* statemachine;
    int* popSize = nullptr;
	
	void Handle() override;
	
	BunnyCheckForPopulationCountMessage(void* ent, void* sm);
};

struct MoveEntityUsingVectorMessage : public msg_base
{
	void* ent;
	float speed;
	float x;
	float y;
	
	void Handle() override;
	
	MoveEntityUsingVectorMessage(void* e, float s, float nx, float ny);
};

struct BunnyFindMateMessage : public msg_base
{
    void* ent;
    void** mate;

    void Handle() override;

    BunnyFindMateMessage(void* entity);
};

#endif
