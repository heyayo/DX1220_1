#include "maze.hpp"

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

RaycastHitInfo Maze::raycast(vec2 origin, vec2 end)
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
    for (const auto x : tiles)
    {
        if (x.first < 0 || x.second < 0) continue;
        if (x.first >= _width || x.second >= _height) continue;
        auto& tile = _mazeData[coordToIndex(x)];
        info.hits.emplace_back(
                    tile,
                    x
                );
        if (tile.entity)
        {
            if (tile.entity->modifier == SOLID)
                info.firstHit = &tile;
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
    int index = coordToIndex(ent->pos);
    auto entTile = _mazeData[index];
    vec2 endPos = ent->pos;
    endPos.first += diff.first;
    endPos.second += diff.second;
    auto rayinfo = raycast(ent->pos, endPos);

    if (rayinfo.hits.empty())
        return;
    if (rayinfo.firstHit) // Check if the raycast hit something
    {
        if (rayinfo.firstHitIndex > 0) // Check if the first hit is not next to the entity
        {
            ent->pos = rayinfo.hits[rayinfo.firstHitIndex-1].second;
            int nextIndex = coordToIndex(ent->pos);
            auto nextTile = _mazeData[nextIndex];

            // Swap Tile Entities;
            entTile.entity = nullptr;
            nextTile.entity = ent;
        }
    }
    else
    {
        ent->pos = (rayinfo.hits.end()-1)->second;
        int nextIndex = coordToIndex(ent->pos);
        auto nextTile = _mazeData[nextIndex];

        // Swap Tile Entities;
        entTile.entity = nullptr;
        nextTile.entity = ent;
    }
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

// Following https://www.geeksforgeeks.org/a-search-algorithm/
bool Maze::pathfind(EntityLite* ent, vec2 end, std::vector<vec2>& output)
{
    auto DestCheck = [&](vec2 cell)
    { return cell.first == end.first && cell.second == end.second; };
    auto FillOutput = [&](const std::vector<AStarCell>& details)
    {
        vec2 node = end;
        while (details[coordToIndex(node)].prev != node)
        {
            output.emplace_back(node);
            node = details[coordToIndex(node)].prev;
        }
        output.emplace_back(node);
    };
    auto CalculateH = [&](vec2 n)
    {
        return std::sqrt(
                (n.first - end.first) * (n.first - end.first) +
                        (n.second - end.second) * (n.second - end.second)
                );
    };
    std::vector<bool> closed(getTileCount(),false);
    std::vector<AStarCell> cellData(getTileCount());

    vec2 node = ent->pos;
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
        if (BoundsCheck(upNode))
        {
            if (DestCheck(upNode))
            {
                cellData[coordToIndex(upNode)].prev = node;
                FillOutput(cellData);
                return true;
            }
            if (!closed[coordToIndex(upNode)])
            {
                auto cell = cellData[coordToIndex(upNode)];
                g = cell.g + 1.f;
                h = CalculateH(upNode);
                f = g+h;
            }
        }
    }

    return false;
}

float Maze::GetHeuristic(vec2 start, vec2 end)
{
    vec2 diff = start;
    start.first -= end.first;
    start.second -= end.second;

    auto aSQ = diff.first * diff.first;
    auto bSQ = diff.second * diff.second;
    auto cSQ = aSQ + bSQ;
    return std::sqrt(static_cast<float>(cSQ));
}

bool Maze::BoundsCheck(vec2 pos)
{
    // return true if within the bounds of the maze
    return !(pos.first < 0 || pos.second < 0 || pos.first >= _width || pos.second >= _height);
}
