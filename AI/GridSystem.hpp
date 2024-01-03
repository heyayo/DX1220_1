#ifndef DX1220_GRIDSYSTEM_HPP
#define DX1220_GRIDSYSTEM_HPP

#include "Entities/entity.hpp"

#include <vector>

using vec2 = std::pair<int,int>;

class GridSystem
{
    struct CellData
    {
        std::vector<Entity*> entities;
        unsigned texture;
    };

    int _gridWidth = 0;
    int _gridHeight = 0;
    float _cellWidth = 0.f;
    float _cellHeight = 0.f;

	std::vector<Entity*> _allEntities;
    CellData* _grid = nullptr;

    int GetCellIndexFromEntity(Entity* ent) const;
    int GetCellIndexFromPosition(const Vector3& pos) const;
    int AboveIndex(int index);
    int BottomIndex(int index);
    int LeftIndex(int index);
    int RightIndex(int index);
    Entity* SpiralSearch(Entity* ent, int startingIndex, int depth);
	Entity* SpiralSearch(Entity* ent, const char* tag, int startingIndex, int depth);
    Entity* SpiralSearch(Entity* ent, const std::vector<Entity*>& exceptions, int startingIndex, int depth);
    Entity* SpiralSearch(Entity* ent, const char* tag, const std::vector<Entity*>& exceptions, int startingIndex, int depth);
    Entity* FindEntity(Entity* ent, int index);
    Entity* FindEntity(Entity* ent, const char* tag, int index);
    Entity* FindEntity(Entity* ent, const std::vector<Entity*>& exceptions, int index);
    Entity* FindEntity(Entity* ent, const std::vector<Entity*>& exceptions, const char* tag, int index);

    std::vector<Entity*>::iterator GetEntityIteratorFromCell(Entity* ent, int index);

public:
	~GridSystem();
	
    void init(int gridWidth, int gridHeight, float cellWidth, float cellHeight);

    constexpr int cellCount() const
    { return _gridWidth * _gridHeight; }
    constexpr CellData* getGrid()
    { return _grid; }
    constexpr std::pair<int,int> getGridSize() const
    { return {_gridWidth, _gridHeight }; }
    constexpr std::pair<float,float> getCellSize() const
    { return { _cellWidth, _cellHeight }; }
	constexpr const std::vector<Entity*>& getAllEntities()
	{ return _allEntities; }
    CellData& indexInto(int index);
	
    Entity* spawnEntity(Mesh* mesh, unsigned tex);
    Entity* spawnEntity(Mesh* mesh, unsigned tex, const Vector3& loc);
	void insertEntity(Entity* ent);
    void despawnEntity(Entity* ent);

    void moveEntity(Entity* ent, const Vector3& offset);
	void moveEntityAlongGrid(Entity* ent, const Vector3& loc, float tileSpeed);
    void teleportEntity(Entity* ent, const Vector3& newPos);

    Entity* findClosestEntity(Entity* ent, int radius);
	Entity* findClosestEntity(Entity* ent, const char* tag, int radius);
    Entity* findClosestEntity(Entity* ent, const std::vector<Entity*>& exceptions, int radius);
    Entity* findClosestEntity(Entity* ent, const char* tag, const std::vector<Entity*>& exceptions, int radius);
    std::vector<Entity*> getAllWithTag(const char* tag);

    std::pair<int,int> getCoordinatesFromIndex(int index);
};


#endif
