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

// Replace with your own later
bool Maze::pathfind(EntityLite* ent, vec2 end, std::vector<vec2>& output)
{
    output.clear();
    const vec2 start = ent->pos;

    //gScore stores the total walking distance from startNode to the current node in the search
    std::vector<float> gScore(getTileCount(), FLT_MAX);
    //fScore is defined as "actual distance from startNode to curr + "estimated distance from curr to endNode"
    //i.e. fScore = gScore + Heuristic. every node has a score.
    std::vector<float> fScore(getTileCount(), FLT_MAX);
    //prev just stores where the agent came from before reaching this node in the search
    std::vector<int> prev(getTileCount(), -1);

    //stores nodes for exploring/searching
    //NOTE: ideally, this should be implemented using a priorityqueue
    // the reason is because in A*, the next node we want to explore is always the one with the smallest
    // estimated cost, NOT necessarily the one that was pushed into the queue first
    // (cost in this context is defined as the total distance it'll take to go from start pt to destination)
    //For the ease of implementation, I'll just use an std::list at the (possible) cost of performance.
    std::list<vec2> nodeQueue;
    nodeQueue.push_back(start);
    gScore[coordToIndex(start)] = 0; //the distance from start node to start node is ZERO
    fScore[coordToIndex(start)] = GetHeuristic(start, end); //get a rough estimate of the distance it'll take to walk from startnode to endnode

    //visit each node in the queue until we find a path to destination
    while (!nodeQueue.empty())
    {
        //get node with the lowest fscore from nodeQueue
        //lowest fscore meant the node has a higher chance of being in the shortest path to destination
        auto it =
                std::min_element(nodeQueue.begin(), nodeQueue.end(), [&, fScore](vec2 lhs, vec2 rhs) {
                    return fScore[coordToIndex(lhs)] < fScore[coordToIndex(rhs)];
                });

        vec2 curr = *it;

        //check if path is found
        if (curr == end)
        {
            //constructing our path backwards
            while (curr != start)
            {
                //inserting at the front of a vector isn't efficient.
                //but our path is short enough that we don't really need to care
                output.insert(output.begin(), curr);
                curr = indexToCoord(prev[coordToIndex(curr)]);
            }
            output.insert(output.begin(), ent->pos);
            return true;
        }

        //remove curr from 'queue'
        nodeQueue.erase(it);

        //go through every neighbouring nodes and add them to queue if they provide a better path(by comparing gscore)
        for (Edge& edge : curr->edges)
        {
            Node* neighbour = edge.to;
            //remember, the gscore is the walking distance from startnode to this neighbour
            //we always priortize the lowest scoring edges to be part of the shortest path
            float tempG = gScore[curr->id] + edge.cost; //+edge.cost because that's the distance from curr to neighbour
            if (tempG < gScore[neighbour->id])
            {
                prev[neighbour->id] = curr->id; //record the path!
                gScore[neighbour->id] = tempG;
                fScore[neighbour->id] = tempG + GetHeuristic(neighbour, endNode);
                //add neighbour into 'queue' if it's not already added
                //we will eventually visit this node to explore its neighbours
                if (std::find(nodeQueue.begin(), nodeQueue.end(), neighbour) == nodeQueue.end())
                    nodeQueue.push_back(neighbour);
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
    return std::sqrtf(static_cast<float>(cSQ));
}
