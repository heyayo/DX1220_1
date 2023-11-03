#include "GridSystem.hpp"

#include <algorithm>

GridSystem::~GridSystem()
{
	if (_grid)
		delete[] _grid;
	for (auto e : _allEntities)
		if (e) delete e;
}

void GridSystem::init(int gridWidth, int gridHeight, float cellWidth, float cellHeight)
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

Entity* GridSystem::spawnEntity(Mesh* mesh)
{
    return spawnEntity(mesh, {});
}

Entity* GridSystem::spawnEntity(Mesh* mesh, const Vector3 &loc)
{
    auto ent = new Entity(mesh);
    _grid[0].entities.emplace_back(ent);
	_allEntities.emplace_back(ent);
    teleportEntity(ent,loc);
    return ent;
}

// Moves Entity by Vector
void GridSystem::moveEntity(Entity* ent, const Vector3 &offset)
{
    teleportEntity(ent, ent->getPosition() + offset);
}

// Sets Entity Position to Vector
void GridSystem::teleportEntity(Entity* ent, const Vector3 &newPos)
{
    // Teleport Entity
    int oldIndex = GetCellIndexFromEntity(ent); // Get Entity's Old Cell Index
    ent->setPosition(newPos); // Change Position
    int newIndex = GetCellIndexFromEntity(ent); // Get Entity's New Cell Index

    if (oldIndex == newIndex) return;

    // Remove Entity from Old Cell
    auto& oldCell = _grid[oldIndex].entities; // Old Cell Vector
    auto oldIter = GetEntityIteratorFromCell(ent, oldIndex); // Entity's Old Position in Vector
    oldCell.erase(oldIter); // Remove
    // Add Entity to New Cell
    _grid[newIndex].entities.emplace_back(ent);
}

Entity* GridSystem::findClosestEntity(Entity* ent, int radius)
{
    int startingIndex = GetCellIndexFromEntity(ent);

    for (int i = 1; i <= radius-1; ++i)
    {
        auto target = SpiralSearch(ent, startingIndex, i);
        if (target) return target;
    }

    return nullptr;
}

Entity* GridSystem::SpiralSearch(Entity* ent, int startingIndex, int depth)
{
    Entity* returnVal = nullptr;

    int curr = startingIndex;
    int doubleDepth = depth*2;

    for (int i = 0; i < depth; ++i)
        curr = AboveIndex(curr);
    returnVal = CheckForCandidate(ent, curr);
    if (returnVal) return returnVal;
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = BottomIndex(curr);
        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = LeftIndex(curr);
        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = AboveIndex(curr);
        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }

    return returnVal;
}

Entity* GridSystem::CheckForCandidate(Entity* ent, int index)
{
    auto& cell = _grid[index].entities;
    if (!cell.empty())
        for (auto e : cell)
            if (ent != e) return e;
    return nullptr;
}

std::vector<Entity*>::iterator GridSystem::GetEntityIteratorFromCell(Entity* ent, int index)
{
    auto& vec = _grid[index].entities;
    auto iter = std::find(vec.begin(),vec.end(),ent);
    return iter;
}

int GridSystem::GetCellIndexFromEntity(Entity* ent) const
{
    return GetCellIndexFromPosition(ent->getPosition());
}

int GridSystem::GetCellIndexFromPosition(const Vector3 &pos) const
{
    int xIndex = static_cast<int>(pos.x/_cellWidth);
    int yIndex = static_cast<int>(pos.y/_cellHeight);
    return xIndex + (_gridWidth * yIndex);
}

int GridSystem::AboveIndex(int index)
{ return index + _gridWidth; }
int GridSystem::BottomIndex(int index)
{ return index - _gridWidth; }
int GridSystem::LeftIndex(int index)
{ return index - 1; }
int GridSystem::RightIndex(int index)
{ return index + 1; }
