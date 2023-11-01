#include "GridSystem.hpp"

#include <algorithm>

GridSystem& GridSystem::GetInstance()
{
    static GridSystem _instance;
    return _instance;
}

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
    _grid = new TileData[gridWidth * gridHeight];
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
    int oldIndex = GetIndexOfEntity(ent); // Get Entity's Old Cell Index
    ent->setPosition(newPos); // Change Position
    int newIndex = GetIndexOfEntity(ent); // Get Entity's New Cell Index

    if (oldIndex == newIndex) return;

    // Remove Entity from Old Cell
    auto& oldCell = _grid[oldIndex].entities; // Old Cell Vector
    auto oldIter = GetEntityGridLocation(ent, oldIndex); // Entity's Old Position in Vector
    oldCell.erase(oldIter); // Remove
    // Add Entity to New Cell
    _grid[newIndex].entities.emplace_back(ent);
}

std::vector<Entity*>::iterator GridSystem::GetEntityGridLocation(Entity* ent, int index)
{
    auto& vec = _grid[index].entities;
    auto iter = std::find(vec.begin(),vec.end(),ent);
    return iter;
}

int GridSystem::GetIndexOfEntity(Entity* ent) const
{
    return GetIndexFromPosition(ent->getPosition());
}

int GridSystem::GetIndexFromPosition(const Vector3 &pos) const
{
    int xIndex = static_cast<int>(pos.x/_cellWidth);
    int yIndex = static_cast<int>(pos.y/_cellHeight);
    return xIndex + (_gridWidth * yIndex);
}
