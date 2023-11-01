#ifndef DX1220_GRIDSYSTEM_HPP
#define DX1220_GRIDSYSTEM_HPP

#include "Entities/entity.hpp"

#include <vector>

class GridSystem
{
    struct TileData
    {
        std::vector<Entity*> entities;
    };

    int _gridWidth = 0;
    int _gridHeight = 0;
    float _cellWidth = 0.f;
    float _cellHeight = 0.f;

    TileData* _grid = nullptr;

    int GetIndexOfEntity(Entity* ent) const;
    int GetIndexFromPosition(const Vector3& pos) const;

    std::vector<Entity*>::iterator GetEntityGridLocation(Entity* ent, int index);

public:
    static GridSystem& GetInstance();

    void Init(int gridWidth, int gridHeight, float cellWidth, float cellHeight);

    constexpr int CellCount() const
    { return _gridWidth * _gridHeight; }
    constexpr TileData* GetGrid()
    { return _grid; }

    Entity* SpawnEntity(Mesh* mesh);
    Entity* SpawnEntity(Mesh* mesh, const Vector3& loc);

    void MoveEntity(Entity* ent, const Vector3& offset);
    void TeleportEntity(Entity* ent, const Vector3& newPos);

    constexpr std::pair<int,int> GetGridSize() const
    { return {_gridWidth, _gridHeight }; }
    constexpr std::pair<float,float> GetCellSize() const
    { return { _cellWidth, _cellHeight }; }
};


#endif
