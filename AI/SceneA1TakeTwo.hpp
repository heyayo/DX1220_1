#ifndef DX1220_SCENEA1TAKETWO_HPP
#define DX1220_SCENEA1TAKETWO_HPP

#include "SceneBase.h"

#include "FSMs/Fsm.hpp"
#include "GridSystem.hpp"

class SceneA1TakeTwo : public SceneBase
{
    Mtx44 _projectionMatrix;
    Mtx44 _viewMatrix;

    Mesh* _whiteSquareMesh;
    Mesh* _blackSquareMesh;
    Mesh* _meleeUnitMesh;

    static GridSystem _leftTeamGrid;
    static GridSystem _rightTeamGrid;

    unsigned LoadImage(const char* filepath);
    void MoveCamera(const Vector3& offset);

    void RenderGrid();

    std::vector<FSM> _fsms;

    template<typename T>
    void SpawnEnemyAt(Mesh* mesh, const Vector3 &pos, GridSystem &team);

public:
    virtual void Init();
    virtual void Update(double deltaTime);
    virtual void Render();
    virtual void Exit();
};


#endif
