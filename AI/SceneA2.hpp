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
    EntityLite _brickWall;
    const vec2 hardcoded_map_endpoint{19,29};
    std::string _maps[3];
    size_t _currentMapIndex = 0;

    std::vector<EntityLite*> _mobs;
    size_t _turn = 0;
    bool _playerTurn = true;
    bool _playerLost = false;
    const int _playerActionPointsStandard = 10;

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
    void GenerateMap(const std::string& map_file);

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;
};

/*
 * TODO LIST
 * Swap Enemies for StateMachines to allow for IDLE Wander, Sight Checking and Damage Attacks as opposed to currently insta kill
 * Add Extra Movement Tiles
 * Add Lightning Strikes (Reveal whole map temporarily)
 * Actions towards other entities and world Entities (Use Dice Roll Mechanic)
 * Random Maze Paths
 * Enemy States:
 *  Attack on Sight (Raycast Sight Checking) (Retain sight for x turns)
 *  Patrol (Randomize Direction and travel in a straight line)
 */

#endif //DX1220_SCENE_A_2_HPP
