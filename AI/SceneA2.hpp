#ifndef DX1220_SCENE_A_2_HPP
#define DX1220_SCENE_A_2_HPP

#include "SceneBase.h"
#include "maze.hpp"

#include "GLFW/glfw3.h"

class SceneA2 : public SceneBase
{
    int windowWidth;
    int windowHeight;

    Mesh* _square;
    Maze _maze;

    EntityLite* _player;
    EntityLite* _uniformWall;
    EntityLite _brickWall;

    void RenderMaze();
    void RenderEntities();
    void MoveCamera(const Vector3& offset);

    vec2 GetMousePosition();

    void DEBUG_Raycast();

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;
};

#endif //DX1220_SCENE_A_2_HPP
