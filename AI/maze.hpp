#ifndef DX1220_MAZE_HPP
#define DX1220_MAZE_HPP

#include "entitylite.hpp"

#include <unordered_map>
#include <vector>
#include <cmath>
#include <string>

using vec2 = std::pair<int,int>;

struct MazeTile
{
    EntityLite* entity = nullptr;
    unsigned texture = 0;
};

struct RaycastHitInfo
{
    typedef std::pair<MazeTile&,vec2> result;
    std::vector<result> hits;
    MazeTile* firstHit = nullptr;
    int firstHitIndex = 0;
};

class Maze
{
    int _width = 0;
    int _height = 0;
    int _larger = 0;

    MazeTile* _mazeData = nullptr;
    std::vector<EntityLite*> _entities;

    int FindNearestEmpty(int start);
    float GetHeuristic(vec2 vec);
    bool WithinBounds(vec2 pos);
    bool V2E(vec2 a, vec2 b);
    vec2 V2Minus(vec2 a, vec2 b);

    int up(int i) const;
    vec2 up(vec2 i) const;
    int down(int i) const;
    vec2 down(vec2 i) const;
    int left(int i) const;
    vec2 left(vec2 i) const;
    int right(int i) const;
    vec2 right(vec2 i) const;

    std::vector<vec2> raycastLow(vec2 origin, vec2 end);
    std::vector<vec2> raycastHigh(vec2 origin, vec2 end);

public:

    void init(int w, int h, unsigned base_texture = 0);
    void init(int w, int h, const std::string &csvFile, MazeTile* lookupTable);

    constexpr const std::vector<EntityLite*>& getEntities()
    { return _entities; }
    constexpr vec2 getSize() const
    { return {_width, _height}; }
    constexpr int getTileCount() const
    { return _width * _height; }
    constexpr vec2 indexToCoord(int index)
    {
        int yPos = index / _width;
        int xPos = index % _width;
        return {xPos,yPos};
    }
    constexpr int coordToIndex(vec2 coord)
    { return static_cast<int>(coord.first) + (_width * static_cast<int>(coord.second)); }
    constexpr int tileToIndex(const MazeTile& tile)
    {
        return &tile - _mazeData;
    }
    constexpr const MazeTile& operator[](int index)
    { return _mazeData[index]; }
    EntityLite* spawnEntity(vec2 pos = {})
    {
        auto* e = new EntityLite;
        e->pos = pos;
        _entities.emplace_back(e);
        return e;
    }
    constexpr bool isTileEmpty(int index)
    { return _mazeData[index].entity != nullptr; }

    RaycastHitInfo raycast(vec2 origin, vec2 end);
    bool pathfind(EntityLite* ent, vec2 end, std::vector<vec2>& output);

    void moveEntity(EntityLite* ent, vec2 diff);

    static double length(vec2 vec)
    {
        return std::sqrt(
                (vec.first) * (vec.first) +
                (vec.second) * (vec.second)
        );
    }
};


#endif //DX1220_MAZE_HPP
