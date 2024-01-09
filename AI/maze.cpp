#include "maze.hpp"
#include "rapidcsv.h"

#include <iostream>
#include <algorithm>
#include <cmath>

void Maze::init(int w, int h, unsigned int base_texture)
{
    _width = w; _height = h;
    _larger = _width > _height ? _width : _height;

    delete _mazeData;
    _mazeData = new MazeTile[_width * _height];

    auto count = getTileCount();
    if (!base_texture) return;
    for (int i = 0; i < count; ++i)
        _mazeData[i].texture = base_texture;
}

void Maze::init(int w, int h, const std::string &csvFile, MazeTile* lookupTable)
{
    _width = w; _height = h;
    _larger = _width > _height ? _width : _height;

    delete _mazeData;
    _mazeData = new MazeTile[_width * _height];

    rapidcsv::Document mapFile(csvFile,rapidcsv::LabelParams(-1,-1));
    for (int i = 0; i < _width; ++i)
    {
        auto close = mapFile.GetColumn<int>(i);
        for (int j = 0; j < _height; ++j)
        {
            _mazeData[coordToIndex({i,j})] = lookupTable[close[_height - j - 1]];
        }
    }
}

int Maze::FindNearestEmpty(int start)
{
    int nearest = 0;
    const int grid_boundary = getTileCount();

    int curr = start;
    int doubleDepth = _larger * 2;

    for (int i = 0; i < _larger; ++i)
        curr = up(curr);
    if (curr >= grid_boundary || curr < 0)
        nearest = -1;
    else
        nearest = curr;
    if (isTileEmpty(nearest)) return nearest;
    for (int i = 0; i < _larger; ++i)
    {
        curr = right(curr); // Travel Index
        if (curr >= grid_boundary || curr < 0) // Check if Index Goes Out of Bounds
        {
            nearest = -1; // Set Found Entity to nullptr
            continue; // Skip Candidate Checks | There are no Candidates | Continue instead of Break to keep continuity of Spiral Search
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = down(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = left(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = up(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < _larger; ++i)
    {
        curr = right(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }

    return nearest;
}

int Maze::up(int i) const
{ return i + _width; }
int Maze::down(int i) const
{ return i - _width; }
int Maze::left(int i) const
{ return i - 1; }
int Maze::right(int i) const
{ return i + 1; }
vec2 Maze::up(vec2 i) const
{ return {i.first,i.second+1}; }
vec2 Maze::down(vec2 i) const
{ return {i.first,i.second-1}; }
vec2 Maze::left(vec2 i) const
{ return {i.first-1,i.second}; }
vec2 Maze::right(vec2 i) const
{ return {i.first+1,i.second}; }

RaycastHitInfo Maze::raycast(vec2 origin, vec2 end, EntityLite *ignore)
{
    std::vector<vec2> tiles;
    if (abs(end.second - origin.second) < abs(end.first - origin.first))
    {
        if (origin.first > end.first)
        {
            tiles = raycastLow(end, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastLow(origin, end);
            tiles.emplace_back(end);
        }
    }
    else
    {
        if (origin.second > end.second)
        {
            tiles = raycastHigh(end, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastHigh(origin, end);
            tiles.emplace_back(end);
        }
    }
    RaycastHitInfo info;
    if (tiles.empty())
        return info;
    bool firstHitHit = false;
    for (int i = 0; i < tiles.size(); ++i)
    {
        const auto& x = tiles[i];
        if (x.first < 0 || x.second < 0) continue;
        if (x.first >= _width || x.second >= _height) continue;
        auto& tile = _mazeData[coordToIndex(x)];
        info.hits.emplace_back(
                    tile,
                    x
                );
        if (tile.entity && tile.entity != ignore && !firstHitHit)
        {
            firstHitHit = true;
            info.firstHit = &tile;
            info.firstHitIndex = i;
        }
    }

    return info;
}

std::vector<vec2> Maze::raycastLow(vec2 origin, vec2 end)
{
    std::vector<vec2> tiles;

    int dx = end.first - origin.first;
    int dy = end.second - origin.second;
    int y = origin.second;
    int x_increment = dx > 0 ? 1 : -1;
    int y_increment = 1;
    if (dy < 0)
    {
        y_increment = -1;
        dy = -dy;
    }
    int D = 2*dy - dx;

    for (int x = origin.first; x != end.first; x += x_increment)
    {
        if (D > 0)
        {
            y = y + y_increment;
            D = D + (2 * (dy - dx));
        } else
            D = D + 2*dy;

        tiles.emplace_back(x,y);
    }

    return tiles;
}

std::vector<vec2> Maze::raycastHigh(vec2 origin, vec2 end)
{
    std::vector<vec2> tiles;

    int dx = end.first - origin.first;
    int dy = end.second - origin.second;
    int x_increment = 1;
    int y_increment = dy > 0 ? 1 : -1;
    if (dx < 0)
    {
        x_increment = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = origin.first;

    for (int y = origin.second; y != end.second; y += y_increment)
    {
        if (D > 0)
        {
            x = x + x_increment;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;

        tiles.emplace_back(x,y);
    }

    return tiles;
}

void Maze::moveEntity(EntityLite* ent, vec2 diff)
{
    // Get Entity
    MazeTile* entTile = _entityLocations[ent];
    vec2 entityPosition = indexToCoord(tileToIndex(entTile));
    vec2 endPos = entityPosition;
    endPos.first += diff.first;
    endPos.second += diff.second;
    auto rayinfo = raycast(entityPosition, endPos, ent);

    if (rayinfo.hits.empty())
        return;
    if (rayinfo.firstHit) // Check if the raycast hit something
    {
        if (rayinfo.firstHit->entity != ent)
        {
            if (rayinfo.firstHitIndex > 0) // Check if the first hit is not next to the entity
            {
                entityPosition = rayinfo.hits[rayinfo.firstHitIndex-1].second;
                int nextIndex = coordToIndex(entityPosition);
                auto& nextTile = _mazeData[nextIndex];

                if (nextTile.entity)
                    if (nextTile.entity->modifier != PUSHABLE)
                        return;

                // Swap Tile Entities;
                entTile->entity = nextTile.entity;
                nextTile.entity = ent;
                _entityLocations[ent] = &nextTile;
                return;
            }
        }
    }

    entityPosition = (rayinfo.hits.end()-1)->second;
    int nextIndex = coordToIndex(entityPosition);
    auto& nextTile = _mazeData[nextIndex];

    if (nextTile.entity)
        if (nextTile.entity->modifier != PUSHABLE)
            return;

    // Swap Tile Entities;
    entTile->entity = nextTile.entity;
    nextTile.entity = ent;
    _entityLocations[ent] = &nextTile;
}

void Maze::teleportEntity(EntityLite *ent, vec2 end)
{
    // Get Entity Tiles
    MazeTile* entTile = _entityLocations[ent];
    int nextIndex = coordToIndex(end);
    auto& nextTile = _mazeData[nextIndex];

    // Swap Tile Entities
    entTile->entity = nextTile.entity;
    nextTile.entity = ent;
    _entityLocations[ent] = &nextTile;
}

#include <set>
#define NOMINMAX // Prevent windows.h min and max macros
#include <limits>

struct AStarCell
{
    vec2 prev {-1,-1};
    float f {std::numeric_limits<float>::max()};
    float g {std::numeric_limits<float>::max()};
    float h {std::numeric_limits<float>::max()};
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
// Following https://www.geeksforgeeks.org/a-search-algorithm/
bool Maze::pathfind(EntityLite* ent, vec2 end, std::vector<vec2>& output)
{
    std::vector<bool> closed(getTileCount(),false);
    std::vector<AStarCell> cellData(getTileCount());

    vec2 entityPosition = indexToCoord(tileToIndex(_entityLocations[ent]));

    vec2 node = entityPosition;
    auto& startingCell = cellData[coordToIndex(node)];
    startingCell.f = 0.f;
    startingCell.g = 0.f;
    startingCell.h = 0.f;
    startingCell.prev = node;

    std::set<std::pair<double,vec2>> open;
    open.insert(std::make_pair(0.0, node));

    while (!open.empty())
    {
        auto p = *open.begin();
        open.erase(open.begin());

        node = p.second;
        closed[coordToIndex(node)] = true;

        double f,g,h;

        auto upNode = up(node);
        auto leftNode = left(node);
        auto rightNode = right(node);
        auto downNode = down(node);
        vec2 nodes[] = {upNode,leftNode,rightNode,downNode};
        for (auto& nextNode : nodes)
        {
            if (withinBounds(nextNode))
            {
                if (_mazeData[coordToIndex(nextNode)].entity)
                    if (_mazeData[coordToIndex(nextNode)].entity->modifier != PUSHABLE) continue;
                if (V2E(nextNode, end))
                {
                    cellData[coordToIndex(nextNode)].prev = node;
                    vec2 a = end;
                    while (cellData[coordToIndex(a)].prev != a)
                    {
                        output.emplace_back(a);
                        a = cellData[coordToIndex(a)].prev;
                    }
                    output.emplace_back(a);
                    return true;
                }
                if (!closed[coordToIndex(nextNode)])
                {
                    if (_mazeData[coordToIndex(nextNode)].entity)
                    if (_mazeData[coordToIndex(nextNode)].entity->modifier != PUSHABLE) continue;

                    auto& cell = cellData[coordToIndex(nextNode)];
                    g = cell.g + 1.f;
                    h = GetHeuristic(V2Minus(nextNode, end));
                    f = g+h;

                    if (cell.f  >= std::numeric_limits<float>::max() ||
                            cell.f > f)
                    {
                        open.insert(std::make_pair(f, nextNode));

                        cell.f = f;
                        cell.g = g;
                        cell.h = h;
                        cell.prev = node;
                    }
                }
            }
        }
    }

    return false;
}
#pragma clang diagnostic pop

float Maze::GetHeuristic(vec2 vec)
{
    auto aSQ = vec.first * vec.first;
    auto bSQ = vec.second * vec.second;
    auto cSQ = aSQ + bSQ;
    return std::sqrt(static_cast<float>(cSQ));
}

bool Maze::withinBounds(vec2 pos)
{
    // return true if within the bounds of the maze
    return !(pos.first < 0 || pos.second < 0 || pos.first >= _width || pos.second >= _height);
}

bool Maze::V2E(vec2 a, vec2 b)
{ return a.first == b.first && a.second == b.second; }
vec2 Maze::V2Minus(vec2 a, vec2 b)
{ return {b.first - a.first, b.second - a.second}; }

void Maze::print_map()
{
    for (int i = 0; i < _width; ++i)
    {
        for (int j = 0; j < _height; ++j)
        {
            const auto& e = _mazeData[coordToIndex({i,j})];
            int no = 0;
            if (e.entity)
                no = e.texture;
            std::cout << no << '|';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
