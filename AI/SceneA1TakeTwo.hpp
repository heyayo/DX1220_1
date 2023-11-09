#ifndef DX1220_SCENEA1TAKETWO_HPP
#define DX1220_SCENEA1TAKETWO_HPP

#include "SceneBase.h"

#include "GridSystem.hpp"
#include "FSMs/statemachine.hpp"

class SceneA1TakeTwo : public SceneBase
{
    Mtx44 _projectionMatrix;
    Mtx44 _viewMatrix;

    Mesh* _whiteSquareMesh;
    Mesh* _blackSquareMesh;
    Mesh* _normalSquareMesh;
	unsigned int
	_birdTex,_treeTex;

//    static GridSystem _leftTeamGrid;
//    static GridSystem _rightTeamGrid;
    std::vector<StateMachine*> _sms;
    Entity* _presetTrees[4];

    unsigned LoadImage(const char* filepath);
    void MoveCamera(const Vector3& offset);
	
	std::pair<double,double> MousePos();
	std::pair<double,double> ScreenToWorldSpace(double x, double y);
	std::pair<double,double> MousePosWorldSpace();

    void RenderGrid();
	void RenderEntities();

    template<typename T, typename... ARGS>
    void AttachAIToEntity(Entity* ent, ARGS... a);
	void KillAI(StateMachine* machine); // TODO: Implement
    void KillAI(Entity* ent);

public:
    virtual void Init();
    virtual void Update(double deltaTime);
    virtual void Render();
    virtual void Exit();
	
	static GridSystem AllGrid;
};


#endif
