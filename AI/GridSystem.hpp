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

    int GetCellIndexFromEntity(Entity* ent) const;
    int GetCellIndexFromPosition(const Vector3& pos) const;
    int AboveIndex(int index);
    int BottomIndex(int index);
    int LeftIndex(int index);
    int RightIndex(int index);
    Entity* SpiralSearch(Entity* ent, int startingIndex, int depth);
    Entity* CheckForCandidate(Entity* ent, int index);

    std::vector<Entity*>::iterator GetEntityIteratorFromCell(Entity* ent, int index);

public:
    void init(int gridWidth, int gridHeight, float cellWidth, float cellHeight);

    constexpr int cellCount() const
    { return _gridWidth * _gridHeight; }
    constexpr CellData* getGrid()
    { return _grid; }
    constexpr std::pair<int,int> getGridSize() const
    { return {_gridWidth, _gridHeight }; }
    constexpr std::pair<float,float> getCellSize() const
    { return { _cellWidth, _cellHeight }; }

    Entity* spawnEntity(Mesh* mesh);
    Entity* spawnEntity(Mesh* mesh, const Vector3& loc);

    void moveEntity(Entity* ent, const Vector3& offset);
    void teleportEntity(Entity* ent, const Vector3& newPos);

    Entity* findClosestEntity(Entity* ent, int radius);
};


#endif
