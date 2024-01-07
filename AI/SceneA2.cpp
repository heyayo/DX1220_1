#include "SceneA2.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GLFW/glfw3.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

unsigned player_texture;
unsigned grass_texture;
unsigned brick_texture;

void SceneA2::Init()
{
    SceneBase::Init();

    windowWidth = Application::GetWindowWidth();
    windowHeight = Application::GetWindowHeight();

    float aspectRatio = (float)windowHeight/(float)windowWidth;
    // Initializing Projection Matrix
    Mtx44 projectionMatrix{};
    const int cameraViewDistance = 25;
    projectionMatrix.SetToOrtho(
            0,cameraViewDistance,
            0,cameraViewDistance*aspectRatio,
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
    player_texture = Application::LoadImage("Image/sword.png");
    grass_texture = Application::LoadImage("Image/grass.png");
    brick_texture = Application::LoadImage("Image/bricks.png");

    _brickWall.texture = brick_texture;
    //_maze.init(50,50, grass_texture);
    MazeTile lookup[] = {
            MazeTile{nullptr,grass_texture},
            MazeTile{&_brickWall,grass_texture}};
    _maze.init(20,30,"map.csv",lookup);
    auto hits = _maze.raycast({5, 2}, {0, 0}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;
    std::cout << "AA" << std::endl;
    hits = _maze.raycast({0, 0}, {5, 2}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;

    _player = _maze.spawnEntity();
    _player->texture = player_texture;

    std::cout << "PTR DIFF: " << &_maze[0] - &_maze[5] << std::endl;
}

void SceneA2::Update(double deltaTime)
{
    // Debug Camera Movement
    float camSpeed = static_cast<float>(deltaTime) * 40.f;
    if (Application::IsKeyPressed(GLFW_KEY_A))
        MoveCamera({-camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_D))
        MoveCamera({camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_S))
        MoveCamera({0,-camSpeed,0});
    if (Application::IsKeyPressed(GLFW_KEY_W))
        MoveCamera({0,camSpeed,0});

    if (Application::IsKeyPressed(GLFW_KEY_LEFT))
        _maze.moveEntity(_player,{-1,0});
    if (Application::IsKeyPressed(GLFW_KEY_RIGHT))
        _maze.moveEntity(_player,{1,0});
    if (Application::IsKeyPressed(GLFW_KEY_DOWN))
        _maze.moveEntity(_player,{0,-1});
    if (Application::IsKeyPressed(GLFW_KEY_UP))
        _maze.moveEntity(_player,{0,1});

    if (Application::IsMouseJustPressed(0))
        DEBUG_Raycast();
}

void SceneA2::Render()
{
    SceneBase::Render();

    glDisable(GL_DEPTH_TEST);
    RenderMaze();
    //RenderEntities();
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
        if (tile.entity)
        {
            _square->textureID = tile.entity->texture;
            RenderMesh(_square,false);
        }
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
    modelStack.PopMatrix();
}

void SceneA2::RenderEntities()
{
    const auto& entities = _maze.getEntities();
    modelStack.PushMatrix();
    modelStack.Translate(0.5f,0.5f,0);
    for (int i = 0; i < entities.size(); ++i)
    {
        _square->textureID = entities[i]->texture;
        modelStack.PushMatrix();
        vec2 pos = _maze.getEntityPosition(entities[i]);
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

void SceneA2::DEBUG_Raycast()
{
    static std::vector<std::pair<MazeTile&,vec2>> course;

    if (!course.empty())
        for (auto& tile : course)
            tile.first.texture = grass_texture;

    auto mousePos = GetMousePosition();
    auto rayInfo = _maze.raycast(_maze.getEntityPosition(_player), mousePos, nullptr);
    course = rayInfo.hits;
    for (auto& tile : course)
        tile.first.texture = 0;
}

vec2 SceneA2::GetMousePosition()
{
    double x{},y{};
    Application::GetCursorPos(&x,&y);
    x += camera.position.x;
    y += camera.position.y;
    return {x,y};
}
