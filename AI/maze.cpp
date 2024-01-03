#include "maze.hpp"

#include <iostream>
#include <algorithm>

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
            tiles.emplace_back(origin);
        }
    }
    RaycastHitInfo info;
    for (const auto x : tiles)
    {
        info.hits.emplace_back(
                    _mazeData[coordToIndex(x)],
                    x
                );
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

    std::cout << 'x' << origin.first << '|' << 'y' << y << '|' << 'D' << D << std::endl;
    for (int x = origin.first; x != end.first; x += x_increment)
    {
        tiles.push_back({x,y});
        if (D > 0)
        {
            y = y + y_increment;
            D = D + (2 * (dy - dx));
        } else
            D = D + 2*dy;
        std::cout << 'x' << x << '|' << 'y' << y << '|' << 'D' << D << std::endl;
    }
    std::cout << 'x' << end.first << '|' << 'y' << end.second << '|' << 'D' << D << std::endl;

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

    std::cout << 'x' << x << '|' << 'y' << origin.second << '|' << 'D' << D << std::endl;
    for (int y = origin.second; y != end.second; y += y_increment)
    {
        tiles.push_back({x,y});
        if (D > 0)
        {
            x = x + x_increment;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;

        std::cout << 'x' << x << '|' << 'y' << y << '|' << 'D' << D << std::endl;
    }
    std::cout << 'x' << end.first << '|' << 'y' << end.second << '|' << 'D' << D << std::endl;

    return tiles;
}
