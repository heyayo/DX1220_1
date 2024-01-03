#include "SceneA2.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GLFW/glfw3.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

unsigned TEST_TEXTURE;

void SceneA2::Init()
{
    SceneBase::Init();

    windowWidth = Application::GetWindowWidth();
    windowHeight = Application::GetWindowHeight();

    float aspectRatio = (float)windowHeight/(float)windowWidth;
    // Initializing Projection Matrix
    Mtx44 projectionMatrix{};
    projectionMatrix.SetToOrtho(
            0,10,
            0,10*aspectRatio,
            -10,10
    );
    projectionStack.LoadIdentity();
    projectionStack.LoadMatrix(projectionMatrix);

    camera.Init({0,0,1},
                {0,0,0},
                {0,1,0});

    Mtx44 viewMatrix{};
    viewMatrix.SetToLookAt(
            UnpackVector(camera.position),
            UnpackVector(camera.target),
            UnpackVector(camera.up)
    );
    viewStack.LoadIdentity();
    viewStack.LoadMatrix(viewMatrix);

    _square = MeshBuilder::GenerateQuad("Square",{1,1,1}); // Generic Square Mesh
    TEST_TEXTURE = Application::LoadImage("Image/berd.jpg");

    _maze.init(50,50, TEST_TEXTURE);
    auto hits = _maze.raycast({5, 2}, {0, 0});
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;
    std::cout << "AA" << std::endl;
    hits = _maze.raycast({0,0},{5,2});
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;
}

void SceneA2::Update(double deltaTime)
{
    // Debug Camera Movement
    float camSpeed = static_cast<float>(deltaTime) * 100.f;
    if (Application::IsKeyPressed(GLFW_KEY_A))
        MoveCamera({-camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_D))
        MoveCamera({camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_S))
        MoveCamera({0,-camSpeed,0});
    if (Application::IsKeyPressed(GLFW_KEY_W))
        MoveCamera({0,camSpeed,0});

    if (Application::IsKeyPressed(GLFW_KEY_LEFT))
        --player->pos.first;
    if (Application::IsKeyPressed(GLFW_KEY_RIGHT))
        ++player->pos.first;
    if (Application::IsKeyPressed(GLFW_KEY_DOWN))
        --player->pos.second;
    if (Application::IsKeyPressed(GLFW_KEY_UP))
        ++player->pos.second;
}

void SceneA2::Render()
{
    SceneBase::Render();

    RenderMaze();
}

void SceneA2::Exit()
{

}

void SceneA2::RenderMaze()
{
    const int count = _maze.getTileCount();
    modelStack.PushMatrix();
    modelStack.Translate(0.5f,0.5f,0);
    for (int i = 0; i < count; ++i)
    {
        auto pos = _maze.indexToCoord(i);
        auto tile = _maze[i];
        _square->textureID = tile.texture;
        modelStack.PushMatrix();
        modelStack.Translate(pos.first,pos.second,0);
        RenderMesh(_square,false);
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
    modelStack.PopMatrix();
}

void SceneA2::MoveCamera(const Vector3& offset)
{
    // Change Camera Values
    camera.position += offset;
    camera.target += offset;

    // Recalculate based off new values
    Mtx44 viewMatrix;
    viewMatrix.SetToLookAt(
            UnpackVector(camera.position),
            UnpackVector(camera.target),
            UnpackVector(camera.up)
    );
    viewStack.LoadMatrix(viewMatrix);
}
