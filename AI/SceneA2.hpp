#ifndef DX1220_SCENE_A_2_HPP
#define DX1220_SCENE_A_2_HPP

#include "SceneBase.h"
#include "maze.hpp"

#include "GLFW/glfw3.h"

class SceneA2 : public SceneBase
{
    int windowWidth;
    int windowHeight;
    const int cameraViewDistance = 25;
    float aspectRatio;

    Mesh* _square;
    Maze _maze;

    EntityLite* _player;
    EntityLite* _uniformWall;
    EntityLite _brickWall;

    void RenderMaze();
    void RenderMazeWithFog();
    void RenderEntities();
    void MoveCamera(const Vector3& offset);

    vec2 GetMousePosition();

    void DEBUG_Raycast();
    void DEBUG_Pathfind();

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;
};

#endif //DX1220_SCENE_A_2_HPP
