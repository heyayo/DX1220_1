#ifndef DX1220_SCENEA1TAKETWO_HPP
#define DX1220_SCENEA1TAKETWO_HPP

#include "SceneBase.h"

#include "GridSystem.hpp"
#include "bulletboard.hpp"
#include "FSMs/statemachine.hpp"
#include "FSMs/bunnyai.hpp"
#include "FSMs/birdai.hpp"

struct InfoMsgRenderData
{
	std::string text;
	std::pair<float,float> loc;
	float size = 10.f;
};

#define SPAWNER_FUCTION_COUNT 3

class SceneA1TakeTwo : public SceneBase
{
    typedef void(SceneA1TakeTwo::*SpawnFunction)();
    SpawnFunction spawners[SPAWNER_FUCTION_COUNT];
    std::string spawnerNames[SPAWNER_FUCTION_COUNT];
    int spawnerIndex = 0;
    void BirdSpawner();
    void BunnySpawner();
    void BeeSpawner();

    Mtx44 _projectionMatrix;
    Mtx44 _viewMatrix;

    Mesh* _whiteSquareMesh;
	Mesh* _blackSquareMesh;
	static Mesh* _normalSquareMesh;
	static unsigned int
	_birdTex,_bunnyTex,_beehiveTex,_beeTex,_treeTex;
	
	int staticWidth;
	int staticHeight;

//    static GridSystem _leftTeamGrid;
//    static GridSystem _rightTeamGrid;
    Entity* _presetTrees[4];
    Entity* _beeHive = nullptr;

    unsigned LoadImage(const char* filepath);
    void MoveCamera(const Vector3& offset);
	
	static std::pair<double,double> MousePos();
	std::pair<double,double> ScreenToWorldSpace(double x, double y);
	std::pair<double,double> MousePosWorldSpace();

    void RenderGrid();
	void RenderEntities();
    void RenderInfoTexts();

public:
    template<typename T, typename... ARGS>
    static void AttachAIToEntity(Entity* ent, ARGS... a)
	{
		T* machine = new T(ent, a...);
		sms.push_back(machine);
	}
	template<typename T, typename... ARGS>
	static Entity* SpawnAI(ARGS... a)
	{
		// Constexpr Ifs to determine Texture at compile time
		unsigned int texture = _treeTex;
		if constexpr (std::is_same_v<T,BunnyAI>)
			texture = _bunnyTex;
		if constexpr (std::is_same_v<T,BirdAI>)
			texture = _birdTex;
		
		auto ent = new Entity(_normalSquareMesh,texture);
		BulletBoard::GetInstance().AI_Birth_Queue.posts.push_back({new T(ent, a...),ent});
		return ent;
	}
	static void KillAI(StateMachine* machine);
    static void KillAI(Entity* ent);
	static StateMachine* GetSMFromEntity(Entity* ent);
	
	static std::pair<double,double> GetRandomLocationOnMap();

    virtual void Init();
    virtual void Update(double deltaTime);
    virtual void Render();
    virtual void Exit();
	
	static GridSystem                     AllGrid;
    static std::vector<StateMachine*>     sms;
	static std::vector<InfoMsgRenderData> texts;
};


#endif
