#ifndef DX1220_MAZE_HPP
#define DX1220_MAZE_HPP

#include "entitylite.hpp"

#include <unordered_map>

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
};

class Maze
{
    int _width = 0;
    int _height = 0;
    int _larger = 0;

    MazeTile* _mazeData = nullptr;

    int FindNearestEmpty(int start);

    int up(int i) const;
    int down(int i) const;
    int left(int i) const;
    int right(int i) const;

    std::vector<vec2> raycastLow(vec2 origin, vec2 end);
    std::vector<vec2> raycastHigh(vec2 origin, vec2 end);

public:

    void init(int w, int h, unsigned base_texture = 0);

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
    constexpr const MazeTile& operator[](int index)
    { return _mazeData[index]; }
    const EntityLite* spawnEntity(vec2 pos = {})
    {
        auto* e = new EntityLite;
        e->pos = pos;
        return e;
    }
    constexpr bool isTileEmpty(int index)
    { return _mazeData[index].entity != nullptr; }

    RaycastHitInfo raycast(vec2 origin, vec2 end);
};


#endif //DX1220_MAZE_HPP
