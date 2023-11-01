#ifndef DX1220_GRIDSYSTEM_HPP
#define DX1220_GRIDSYSTEM_HPP

#include "Entities/entity.hpp"

#include <vector>

class GridSystem
{
    struct CellData
    {
        std::vector<Entity*> entities;
    };

    int _gridWidth = 0;
    int _gridHeight = 0;
    float _cellWidth = 0.f;
    float _cellHeight = 0.f;

    CellData* _grid = nullptr;

    int getCellIndexFromEntity(Entity* ent) const;
    int getCellIndexFromPosition(const Vector3& pos) const;
    int aboveIndex(int index);
    int bottomIndex(int index);
    int leftIndex(int index);
    int rightIndex(int index);
    Entity* spiralSearch(Entity* ent, int startingIndex, int depth);
    Entity* checkForCandidate(Entity* ent, int index);

    std::vector<Entity*>::iterator getEntityIteratorFromCell(Entity* ent, int index);

public:
    void Init(int gridWidth, int gridHeight, float cellWidth, float cellHeight);

    constexpr int CellCount() const
    { return _gridWidth * _gridHeight; }
    constexpr CellData* GetGrid()
    { return _grid; }
    constexpr std::pair<int,int> GetGridSize() const
    { return {_gridWidth, _gridHeight }; }
    constexpr std::pair<float,float> GetCellSize() const
    { return { _cellWidth, _cellHeight }; }

    Entity* SpawnEntity(Mesh* mesh);
    Entity* SpawnEntity(Mesh* mesh, const Vector3& loc);

    void MoveEntity(Entity* ent, const Vector3& offset);
    void TeleportEntity(Entity* ent, const Vector3& newPos);

    Entity* FindClosestEntity(Entity* ent, int radius);
};


#endif
