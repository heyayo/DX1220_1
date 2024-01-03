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

    EntityLite* player;

    void RenderMaze();
    void MoveCamera(const Vector3& offset);

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;
};

#endif //DX1220_SCENE_A_2_HPP
