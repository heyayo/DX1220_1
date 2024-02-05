#include "SceneA2.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GLFW/glfw3.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) vec.x << ',' << vec.y << ',' << vec.z
#define LOGINFO(msg) std::cout << "[INFO]\t" << msg << std::endl

unsigned player_texture;
unsigned enemy_texture;

unsigned grass_texture;
unsigned brick_texture;
unsigned swamp_texture;
unsigned end_level_texture;

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

    _playerLost = false;

    // Store Map Files to Load
    _maps[0] = {"mapone.csv", &SceneA2::CODEINIT_MapOne, &SceneA2::CODEUPDATE_MapOne};
    _maps[1] = {"maptwo.csv", &SceneA2::CODEINIT_MapTwo, &SceneA2::CODEUPDATE_MapTwo};
    _maps[2] = {"mapthree.csv", &SceneA2::CODEINIT_MapThree, &SceneA2::CODEUPDATE_MapThree};

    _square = MeshBuilder::GenerateQuad("Square",{1,1,1}); // Generic Square Mesh
    _red = MeshBuilder::GenerateQuad("Square",{1,0,0}); // Generic Square Mesh
    player_texture = Application::LoadImage("Image/goku.png");
    grass_texture = Application::LoadImage("Image/grass.png");
    brick_texture = Application::LoadImage("Image/bricks.png");
    swamp_texture = Application::LoadImage("Image/mud.png");
    enemy_texture = Application::LoadImage("Image/freeza.png");
    end_level_texture = Application::LoadImage("Image/endlevel.png");

    _brickWall.texture = brick_texture;
    //_maze.init(50,50, grass_texture);
    lookup = {
            MazeTile{nullptr,grass_texture,1},
            MazeTile{&_brickWall,grass_texture,1},
            MazeTile{nullptr,swamp_texture,2},
            MazeTile{nullptr,grass_texture,1}, // For Freeza Spawning
            MazeTile{nullptr,grass_texture,1}, // For Freeza Spawning ( UNUSED )
            MazeTile{&_brickWall,grass_texture,0}, // TIMER ALTERING WALLS
            MazeTile{nullptr,grass_texture,0}, // No cost Tile
    };
    spawn_data = {
            EnemySpawnData{enemy_texture, 6, 3, 0.1}
    };
    _maze.init(20, 30, _maps[_currentMapIndex].file, lookup, _mobs, spawn_data);
    (this->*_maps[_currentMapIndex].init)(); // Call First MapCode
    auto hits = _maze.raycast({5, 2}, {0, 0}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;
    std::cout << "AA" << std::endl;
    hits = _maze.raycast({0, 0}, {5, 2}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;

    _player = _maze.spawnEntity();
    _player->texture = player_texture;
    _player->modifier = PUSHABLE;
    _player->base_points = _playerActionPointsStandard;
    _player->action_points = _playerActionPointsStandard;
    _player->speed = 0.1;
    _mobs.emplace(_mobs.begin(),_player);

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
    if (Application::IsKeyPressed(GLFW_KEY_SPACE))
    {
        vec2 playerPos = _maze.getEntityPosition(_player);
        camera.position = {static_cast<float>(playerPos.first)-12.5f,static_cast<float>(playerPos.second)-12.5f*aspectRatio,1};
        camera.target = {static_cast<float>(playerPos.first)-12.5f,static_cast<float>(playerPos.second)-12.5f*aspectRatio,0};

        // Recalculate based off new values
        Mtx44 viewMatrix;
        viewMatrix.SetToLookAt(
                UnpackVector(camera.position),
                UnpackVector(camera.target),
                UnpackVector(camera.up)
        );
        viewStack.LoadMatrix(viewMatrix);
    }

    if (Application::IsKeyPressed(GLFW_KEY_LEFT))
        _maze.moveEntity(_player,{-1,0});
    if (Application::IsKeyPressed(GLFW_KEY_RIGHT))
        _maze.moveEntity(_player,{1,0});
    if (Application::IsKeyPressed(GLFW_KEY_DOWN))
        _maze.moveEntity(_player,{0,-1});
    if (Application::IsKeyPressed(GLFW_KEY_UP))
        _maze.moveEntity(_player,{0,1});

    if (_playerTurn)
    {
        PlayerChoice();

        // End Turn
        if (Application::IsMouseJustPressed(1))
        {
            _playerTurn = false;
            LOGINFO("Player Ended Turn");
        }
    }
    else
    {
        if (DoTurn()) // Is Turn Done
        {
            LOGINFO("Turn Ended");
            if (NextTurn()) // Has Turn Cycled
            {
                LOGINFO("Player's Turn");
                _playerTurn = true; // Player's Turn
                _totalTurns += 1;
                (this->*_maps[_currentMapIndex].update)();
            }
        }
    }
    if (_maze.getEntityPosition(_player) == hardcoded_map_endpoint)
    {
        // Next Map
        ++_currentMapIndex;
        _currentMapIndex %= 3;
        GenerateMap(_currentMapIndex);
        _turn = 0;
    }
    if (_playerLost)
    {
        _currentMapIndex = 0;
        GenerateMap(_currentMapIndex);
        _playerLost = false;
        _playerTurn = true;
        _totalTurns = 0;
    }

    (this->*_maps[_currentMapIndex].update)(); // Call Update for this map
}

void SceneA2::Render()
{
    SceneBase::Render();

    modelStack.PushMatrix();
    modelStack.Translate(0.5f,0.5f,0);
    glDisable(GL_DEPTH_TEST);
    //RenderMaze();
    RenderMazeWithFog();
    //DEBUG_Raycast();
    //DEBUG_Pathfind();
    RenderCourseLine(_player->course);
    //RenderEntities();
    modelStack.PopMatrix();

    static const Color textColor{1,.5,.5};
    // Render Text to Tell Player it is their turn
    if (_playerTurn)
    {
        RenderTextOnScreen(meshList[GEO_TEXT],"Player's Turn",textColor,40,0,0);
    }
    else
    {
        RenderTextOnScreen(meshList[GEO_TEXT],"AI Turn",textColor,40,0,0);
    }

    // Render Helper Texts
    std::string AP_warning = std::to_string(_player->action_points) + " Action Points Left";
    RenderTextOnScreen(meshList[GEO_TEXT], AP_warning, textColor, 40, 0, 40);
    std::string warning_text = std::to_string(_sighted.size()) + " Enemies Can See You";
    RenderTextOnScreen(meshList[GEO_TEXT], warning_text, textColor, 40, 0, 80);
    RenderTextOnScreen(meshList[GEO_TEXT],"Left Click To Choose Where To Go",textColor,40,0,120);
    RenderTextOnScreen(meshList[GEO_TEXT],"Right Click To End Turn",textColor,40,0,160);
    RenderTextOnScreen(meshList[GEO_TEXT],"WASD To Move Camera",textColor,40,0,200);
    RenderTextOnScreen(meshList[GEO_TEXT],"Press Space To Center Camera On Player",textColor,40,0,240);
}

void SceneA2::Exit()
{

}

void SceneA2::RenderMaze()
{
    const int count = _maze.getTileCount();
    for (int i = 0; i < count; ++i)
    {
        auto pos = _maze.indexToCoord(i);
        auto tile = _maze[i];
        _square->textureID = tile.texture;
        modelStack.PushMatrix();
        modelStack.Translate(pos.first,pos.second,0);
        modelStack.Rotate(180,0,0,1);
        RenderMesh(_square,false);
        if (tile.entity)
        {
            _square->textureID = tile.entity->texture;
            RenderMesh(_square,false);
        }
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
}

void SceneA2::RenderMazeWithFog()
{
    //int start = _maze.coordToIndex(_maze.getEntityPosition(_player));
    vec2 start = _maze.getEntityPosition(_player);
    for (int i = 0; i < playerFogViewRadius; ++i)
    {
        start = _maze.left(start);
    }
    for (int j = 0; j < playerFogViewRadius; ++j)
    {
        start = _maze.up(start);
    }
    for (int i = 0; i < playerFogViewRadius*2+1; ++i)
    {
        //int col = start;
        vec2 col = start;
        for (int j = 0; j < playerFogViewRadius*2+1; ++j)
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
            modelStack.Rotate(180,0,0,1);
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
}

void SceneA2::RenderFogOverlay()
{
    const int count = _maze.getTileCount();
    for (int i = 0; i < count; ++i)
    {
        auto pos = _maze.indexToCoord(i);
        auto tile = _maze[i];
        _square->textureID = tile.texture;
        modelStack.PushMatrix();
        modelStack.Translate(pos.first,pos.second,0);
        modelStack.Rotate(180,0,0,1);
        RenderMesh(_square,false);
        if (tile.entity)
        {
            _square->textureID = tile.entity->texture;
            RenderMesh(_square,false);
        }
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
}

void SceneA2::RenderEntities()
{
    const auto& entities = _maze.getEntities();
    for (int i = 0; i < entities.size(); ++i)
    {
        _square->textureID = entities[i]->texture;
        modelStack.PushMatrix();
        vec2 pos = _maze.getEntityPosition(entities[i]);
        modelStack.Translate(pos.first,pos.second,0);
        modelStack.Rotate(180,0,0,1);
        RenderMesh(_square,false);
        modelStack.PopMatrix();
        _square->textureID = 0;
    }
}

void SceneA2::RenderCourseLine(const std::vector<vec2>& course)
{
    auto copy = course;
    int i_Max = course.size() > _playerActionPointsStandard ? _playerActionPointsStandard : course.size();
    std::reverse(copy.begin(),copy.end());
    for (int i = 0; i < i_Max; ++i)
    {
        const auto& x = copy[i];
        modelStack.PushMatrix();

        modelStack.Translate(x.first,x.second,0);
        modelStack.Scale(0.2f,0.2f,1);
        RenderMesh(_square,false);

        modelStack.PopMatrix();
    }

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

//        for (const auto& a : course)
//            LOGINFO(a.first << '.' << a.second);
    }

    timer += Application::DELTATIME;
    if (timer > 0.1f && !course.empty())
    {
        timer = 0.f;
        _maze.teleportEntity(_player,*(course.end()-1));
        course.erase(course.end()-1);
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

void SceneA2::PlayerChoice()
{
    if (Application::IsMouseJustPressed(0))
    {
        auto mousePos = GetMousePosition();
        _player->course.clear();
        bool result = _maze.pathfind(_player,mousePos,_player->course);
        if (result) LOGINFO("Path found");
        else LOGINFO("Path blocked");
    }
}

bool SceneA2::NextTurn()
{
    EntityLite* mob = _mobs[_turn];

    mob->action_points = _mobs[_turn]->base_points;
    mob->course.clear();
    size_t old = _turn;
    ++_turn;
    _turn %= _mobs.size();

    // Enemy Thinking Phase
    if (mob != _player)
    {
        auto res = _maze.raycast(_maze.getEntityPosition(mob),_maze.getEntityPosition(_player),mob);
        if (res.firstHit && res.firstHit->entity == _player)
        {
            // Chase State
            _maze.pathfind(mob,_maze.getEntityPosition(_player),mob->course);
            LOGINFO("Player in Line of Sight");
            _sighted.insert(mob);
        }
        else
        {
            // Patrol State
            std::uniform_int_distribution<int> distribution(0,_possiblePatrolPoints.size()-1);
            int random = distribution(Application::randomthing);
            _maze.pathfind(mob,_maze.indexToCoord(_possiblePatrolPoints[random]),mob->course);
            LOGINFO("Enemy Wandering");
            _sighted.erase(mob);
        }
    }
    LOGINFO("TURN: " << _turn);

    return old > _turn;
}

bool SceneA2::DoTurn()
{
    static double timer = 0.0;

    EntityLite* thisTurn = _mobs[_turn];

    if (!thisTurn)
        return true;
    if (thisTurn->course.empty()) return true;

    timer += Application::DELTATIME;
    if (timer > thisTurn->speed)
    {
        vec2 tilePos = *(thisTurn->course.end()-1);
        auto index = _maze.coordToIndex(tilePos);
        const auto& tile = _maze[index];

        if (thisTurn->action_points < tile.cost)
            thisTurn->course.clear(); // End Turn;
        else
        {
            if (thisTurn != _player)
            {
                if (tilePos == _maze.getEntityPosition(_player))
                {
                    // Do before teleporting to ensure that tilePos is indeed the player's position
                    _playerLost = true; // Toggle Game End
                }
            }
            if (tile.entity != thisTurn && tile.entity != nullptr)
            {
                LOGINFO("TILE COLLISION | " << tile.entity << " | " << _player);
                thisTurn->course.clear();
                return true;
            }
            timer = 0;
            _maze.teleportEntity(thisTurn,tilePos);
            thisTurn->course.erase(thisTurn->course.end()-1);
            thisTurn->action_points -= tile.cost;
        }
    }

    return thisTurn->course.empty();
}

void SceneA2::GenerateMap(size_t mapcode)
{
    _sighted.clear();

    _maze.init(20, 30, _maps[mapcode].file, lookup, _mobs, spawn_data);
    (this->*_maps[mapcode].init)();
    auto hits = _maze.raycast({5, 2}, {0, 0}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;
    std::cout << "AA" << std::endl;
    hits = _maze.raycast({0, 0}, {5, 2}, nullptr);
    for (auto x : hits.hits)
        std::cout << x.second.first << ' ' << x.second.second << std::endl;

    _player = _maze.spawnEntity();
    _player->texture = player_texture;
    _player->modifier = PUSHABLE;
    _player->base_points = _playerActionPointsStandard;
    _player->action_points = _playerActionPointsStandard;
    _player->speed = 0.1;
    _mobs.emplace(_mobs.begin(),_player);
}

std::vector<std::vector<size_t>> map_switching_cells;
MazeTile no_cost_tile
        {
    nullptr,
    grass_texture,
    0,99999 // Cell ID Lost After Loading
        };

void SceneA2::CODEINIT_MapOne()
{
    map_switching_cells.clear();
    _possiblePatrolPoints.clear();

    auto five = _maze.getCells(5);
    auto six = _maze.getCells(6);
    _possiblePatrolPoints = _maze.getCells(10);

    map_switching_cells.emplace_back(five);
    map_switching_cells.emplace_back(six);

    // Set the Ground Texture for the End Of Map Marker
    _maze[_maze.coordToIndex(hardcoded_map_endpoint)].texture = end_level_texture;
}

void SceneA2::CODEUPDATE_MapOne()
{
    static int turn_timer = _totalTurns;
    static bool five_or_six = false;

    if (_totalTurns - turn_timer > 2)
    {
        LOGINFO("Turn Diff: " << _totalTurns - turn_timer);
        turn_timer = _totalTurns;
        if (five_or_six)
        {
            five_or_six = false;
            for (auto f : map_switching_cells[1])
            {
                if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
                _maze[f].entity = nullptr;
            }
            for (auto f : map_switching_cells[0])
            {
                if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
                _maze[f].entity = &_brickWall;
            }
        }
        else
        {
            five_or_six = true;
            for (auto f : map_switching_cells[1])
            {
                if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
                _maze[f].entity = &_brickWall;
            }
            for (auto f : map_switching_cells[0])
            {
                if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
                _maze[f].entity = nullptr;
            }
        }
    }
}

void SceneA2::CODEINIT_MapTwo()
{
    map_switching_cells.clear();
    _possiblePatrolPoints.clear();

    auto five = _maze.getCells(5);
    auto six = _maze.getCells(6);
    auto seven = _maze.getCells(7);
    _possiblePatrolPoints = _maze.getCells(10);

    map_switching_cells.emplace_back(five);
    map_switching_cells.emplace_back(six);
    map_switching_cells.emplace_back(seven);

//    for (auto f : five)
//        _maze[f] = no_cost_tile;
//    for (auto f : six)
//        _maze[f] = no_cost_tile;
    for (auto f : seven)
        _maze[f] = _maze[five[0]]; // Place Brick Wall Hacky Code

    // Set the Ground Texture for the End Of Map Marker
    _maze[_maze.coordToIndex(hardcoded_map_endpoint)].texture = end_level_texture;
}

void SceneA2::CODEUPDATE_MapTwo()
{
    static int turn_timer = _totalTurns;
    static int alternator = false;

    if (_totalTurns - turn_timer > 2)
    {
        ++alternator;
        alternator %= 3;
        LOGINFO("Turn Diff: " << _totalTurns - turn_timer);
        turn_timer = _totalTurns;
        LOGINFO("Alternator " << alternator);

        for (auto f : map_switching_cells[0])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = nullptr;
        }
        for (auto f : map_switching_cells[1])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = nullptr;
        }
        for (auto f : map_switching_cells[2])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = nullptr;
        }
        for (auto f : map_switching_cells[alternator])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = &_brickWall;
        }
    }
}

void SceneA2::CODEINIT_MapThree()
{
    map_switching_cells.clear();
    _possiblePatrolPoints.clear();

    auto five = _maze.getCells(5);
    auto six = _maze.getCells(6);
    _possiblePatrolPoints = _maze.getCells(10);

    map_switching_cells.emplace_back(five);
    map_switching_cells.emplace_back(six);

    // Set the Ground Texture for the End Of Map Marker
    _maze[_maze.coordToIndex(hardcoded_map_endpoint)].texture = end_level_texture;
}

void SceneA2::CODEUPDATE_MapThree()
{
    static int turn_timer = _totalTurns;
    static int alternator = false;

    if (_totalTurns - turn_timer > 1)
    {
        ++alternator;
        alternator %= 2;
        LOGINFO("Turn Diff: " << _totalTurns - turn_timer);
        turn_timer = _totalTurns;
        LOGINFO("Alternator " << alternator);

        for (auto f : map_switching_cells[0])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = nullptr;
        }
        for (auto f : map_switching_cells[1])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = nullptr;
        }
        for (auto f : map_switching_cells[alternator])
        {
            if (_maze[f].entity && _maze[f].entity != &_brickWall) continue;
            _maze[f].entity = &_brickWall;
        }
    }
}
