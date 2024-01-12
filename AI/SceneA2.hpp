#ifndef DX1220_SCENE_A_2_HPP
#define DX1220_SCENE_A_2_HPP

#include "SceneBase.h"
#include "maze.hpp"
#include "turnmeter.hpp"

#include "GLFW/glfw3.h"

class SceneA2 : public SceneBase
{
    int windowWidth;
    int windowHeight;
    const int cameraViewDistance = 25;
    float aspectRatio;

    Mesh* _square;
    Maze _maze;
    TurnMeter _turnmeter{_maze};

    EntityLite* _player;
    EntityLite _brickWall;

    std::vector<EntityLite*> _mobs;
    size_t _turn = 0;
    bool _playerTurn = true;

    void RenderMaze();
    void RenderMazeWithFog();
    void RenderEntities();
    void RenderCourseLine(const std::vector<vec2>& course);
    void MoveCamera(const Vector3& offset);

    vec2 GetMousePosition();

    void DEBUG_Raycast();
    void DEBUG_Pathfind();

    void PlayerChoice();
    bool DoTurn();
    bool NextTurn();

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;
};

#endif //DX1220_SCENE_A_2_HPP
