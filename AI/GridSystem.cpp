#include "GridSystem.hpp"

#include <algorithm>

void GridSystem::Init(int gridWidth, int gridHeight, float cellWidth, float cellHeight)
{
    _gridWidth = gridWidth;
    _gridHeight = gridHeight;
    _cellWidth = cellWidth;
    _cellHeight = cellHeight;

    // Re-Initializer Check
    if (_grid)
    {
        delete _grid;
        _grid = nullptr;
    }
    _grid = new CellData[gridWidth * gridHeight];
}

Entity* GridSystem::SpawnEntity(Mesh* mesh)
{
    return SpawnEntity(mesh,{});
}

Entity* GridSystem::SpawnEntity(Mesh* mesh, const Vector3 &loc)
{
    auto ent = new Entity(mesh);
    _grid[0].entities.emplace_back(ent);
    ent->setPosition(loc);
    return ent;
}

// Moves Entity by Vector
void GridSystem::MoveEntity(Entity* ent, const Vector3 &offset)
{
    TeleportEntity(ent,ent->getPosition()+offset);
}

// Sets Entity Position to Vector
void GridSystem::TeleportEntity(Entity* ent, const Vector3 &newPos)
{
    // Teleport Entity
    int oldIndex = getCellIndexFromEntity(ent); // Get Entity's Old Cell Index
    ent->setPosition(newPos); // Change Position
    int newIndex = getCellIndexFromEntity(ent); // Get Entity's New Cell Index

    if (oldIndex == newIndex) return;

    // Remove Entity from Old Cell
    auto& oldCell = _grid[oldIndex].entities; // Old Cell Vector
    auto oldIter = getEntityIteratorFromCell(ent, oldIndex); // Entity's Old Position in Vector
    oldCell.erase(oldIter); // Remove
    // Add Entity to New Cell
    _grid[newIndex].entities.emplace_back(ent);
}

Entity* GridSystem::FindClosestEntity(Entity* ent, int radius)
{
    int startingIndex = getCellIndexFromEntity(ent);

    for (int i = 1; i <= radius; ++i)
    {
        auto target = spiralSearch(ent,startingIndex,i);
        if (target) return target;
    }

    return nullptr;
}

Entity* GridSystem::spiralSearch(Entity* ent, int startingIndex, int depth)
{
    Entity* returnVal = nullptr;

    int curr = startingIndex;
    int doubleDepth = depth*2;

    for (int i = 0; i < depth; ++i)
        curr = aboveIndex(curr);
    returnVal = checkForCandidate(ent,curr);
    if (returnVal) return returnVal;
    for (int i = 0; i < depth; ++i)
    {
        curr = rightIndex(curr);
        returnVal = checkForCandidate(ent,curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = bottomIndex(curr);
        returnVal = checkForCandidate(ent,curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = leftIndex(curr);
        returnVal = checkForCandidate(ent,curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = aboveIndex(curr);
        returnVal = checkForCandidate(ent,curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < depth; ++i)
    {
        curr = rightIndex(curr);
        returnVal = checkForCandidate(ent,curr);
        if (returnVal) return returnVal;
    }

    return returnVal;
}

Entity* GridSystem::checkForCandidate(Entity* ent, int index)
{
    auto& cell = _grid[index].entities;
    if (!cell.empty())
        for (auto e : cell)
            if (ent != e) return e;
    return nullptr;
}

std::vector<Entity*>::iterator GridSystem::getEntityIteratorFromCell(Entity* ent, int index)
{
    auto& vec = _grid[index].entities;
    auto iter = std::find(vec.begin(),vec.end(),ent);
    return iter;
}

int GridSystem::getCellIndexFromEntity(Entity* ent) const
{
    return getCellIndexFromPosition(ent->getPosition());
}

int GridSystem::getCellIndexFromPosition(const Vector3 &pos) const
{
    int xIndex = static_cast<int>(pos.x/_cellWidth);
    int yIndex = static_cast<int>(pos.y/_cellHeight);
    return xIndex + (_gridWidth * yIndex);
}

int GridSystem::aboveIndex(int index)
{ return index + _gridWidth; }
int GridSystem::bottomIndex(int index)
{ return index - _gridWidth; }
int GridSystem::leftIndex(int index)
{ return index - 1; }
int GridSystem::rightIndex(int index)
{ return index + 1; }
