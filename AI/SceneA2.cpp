#include "SceneA2.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GLFW/glfw3.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) vec.x << ',' << vec.y << ',' << vec.z
#define LOGINFO(msg) std::cout << "[INFO]\t" << msg << std::endl

unsigned player_texture;
unsigned grass_texture;
unsigned brick_texture;

Mesh* _red;

void SceneA2::Init()
{
    SceneBase::Init();

    windowWidth = Application::GetWindowWidth();
    windowHeight = Application::GetWindowHeight();

    LOGINFO(windowWidth);
    LOGINFO(windowHeight);

    aspectRatio = (float)windowHeight/(float)windowWidth;
    // Initializing Projection Matrix
    Mtx44 projectionMatrix{};
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
    _red = MeshBuilder::GenerateQuad("Square",{1,0,0}); // Generic Square Mesh
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

    LOGINFO(PrintVector(camera.position));
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

    static bool spaceCheck = false;
    if (Application::IsKeyPressed(GLFW_KEY_SPACE) && !spaceCheck)
    {
        _maze.print_map();
    }
    spaceCheck = Application::IsKeyPressed(GLFW_KEY_SPACE);
}

void SceneA2::Render()
{
    SceneBase::Render();

    glDisable(GL_DEPTH_TEST);
    RenderMaze();
    //DEBUG_Raycast();
    DEBUG_Pathfind();
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

void SceneA2::RenderMazeWithFog()
{
    const int radius = 3;
    modelStack.PushMatrix();
    modelStack.Translate(0.5f,0.5f,0);
    //int start = _maze.coordToIndex(_maze.getEntityPosition(_player));
    vec2 start = _maze.getEntityPosition(_player);
    for (int i = 0; i < radius; ++i)
    {
        start = _maze.left(start);
    }
    for (int j = 0; j < radius; ++j)
    {
        start = _maze.up(start);
    }
    for (int i = 0; i < radius*2+1; ++i)
    {
        //int col = start;
        vec2 col = start;
        for (int j = 0; j < radius*2+1; ++j)
        {
            //auto pos = _maze.indexToCoord(col);
            auto pos = col;
            if (!_maze.withinBounds(pos))
            {
                col = _maze.down(col);
                continue;
            }
            auto tile = _maze[_maze.coordToIndex(pos)];
            //auto tile = _maze[col];
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
            col = _maze.down(col);
        }
        start = _maze.right(start);
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
    static RaycastHitInfo rayInfo;

    if (Application::IsMouseJustPressed(0))
    {
        auto mousePos = GetMousePosition();
        LOGINFO(mousePos.first << '|' << mousePos.second);
        rayInfo = _maze.raycast(_maze.getEntityPosition(_player), mousePos, _player);
        course = rayInfo.hits;
        for (auto x : course)
            LOGINFO("Raycast Path: " << x.second.first << '|' << x.second.second);
        if (rayInfo.firstHit)
            LOGINFO("First Hit: " << course[rayInfo.firstHitIndex].second.first << '|' << course[rayInfo.firstHitIndex].second.second);
        else LOGINFO("No First Hit");
    }

    modelStack.PushMatrix();
    modelStack.Translate(0.5f,0.5f,0);
    for (auto x : course)
    {
        _square->textureID = 0;
        modelStack.PushMatrix();
        modelStack.Translate(x.second.first,x.second.second,0);
        RenderMesh(_square,false);
        modelStack.PopMatrix();
    }
    modelStack.PushMatrix();
    auto pos = rayInfo.firstHit ? course[rayInfo.firstHitIndex].second : vec2{};
    modelStack.Translate(pos.first,pos.second,0);
    RenderMesh(_red,false);
    modelStack.PopMatrix();
    modelStack.PopMatrix();
}

void SceneA2::DEBUG_Pathfind()
{
    static std::vector<vec2> course{};
    static float timer = 0.f;

    if (Application::IsMouseJustPressed(0))
    {
        auto mousePos = GetMousePosition();
        LOGINFO(mousePos.first << '|' << mousePos.second);
        course.clear();
        bool result = _maze.pathfind(_player,mousePos,course);
        if (result) LOGINFO("Path found");
        else LOGINFO("Path blocked");

        for (const auto& a : course)
            LOGINFO(a.first << '.' << a.second);
    }

    timer += Application::DELTATIME;
    if (timer > 0.5f && !course.empty())
    {
        timer = 0.f;
        _maze.teleportEntity(_player,*(course.end()-1));
        course.erase(course.end());
    }

//    modelStack.PushMatrix();
//    modelStack.Translate(0.5f,0.5f,0);
//    for (auto x : course)
//    {
//        _square->textureID = 0;
//        modelStack.PushMatrix();
//        modelStack.Translate(x.first,x.second,0);
//        RenderMesh(_square,false);
//        modelStack.PopMatrix();
//    }
//    modelStack.PopMatrix();
}

vec2 SceneA2::GetMousePosition()
{
    static double x_tile_scale = Application::GetWindowWidth() / cameraViewDistance;
    static double y_tile_scale = Application::GetWindowHeight() / (cameraViewDistance*aspectRatio);

    double x{},y{};
    Application::GetCursorPos(&x,&y);
    y = windowHeight - y;

    x /= x_tile_scale;
    x += camera.position.x;
    y /= x_tile_scale;
    y += camera.position.y;
    return {x,y};
}
