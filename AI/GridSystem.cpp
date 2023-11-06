#include "GridSystem.hpp"

#include <algorithm>
#include <cstring>

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

Entity* GridSystem::spawnEntity(Mesh* mesh, unsigned tex)
{
    return spawnEntity(mesh, tex, {});
}

Entity* GridSystem::spawnEntity(Mesh* mesh, unsigned tex, const Vector3& loc)
{
    auto ent = new Entity(mesh,tex);
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

void GridSystem::moveEntityAlongGrid(Entity* ent, const Vector3& loc, float tileSpeed)
{
	Vector3 pos = ent->getPosition();
	Vector3 diff = loc - pos;
	float xDelta = fabs(diff.x);
	float yDelta = fabs(diff.y);
	if (xDelta > yDelta)
		teleportEntity(ent,pos+Vector3{diff.x > 0 ? tileSpeed : -tileSpeed,0,0});
	else
		teleportEntity(ent,pos+Vector3{0,diff.y > 0 ? tileSpeed : -tileSpeed,0});
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

Entity* GridSystem::findClosestEntity(Entity* ent, const char* tag, int radius)
{
	int startingIndex = GetCellIndexFromEntity(ent);
	
	for (int i = 1; i <= radius-1; ++i)
	{
		auto target = SpiralSearch(ent, tag, startingIndex, i);
		if (target)
		{
			if (!strcmp(target->getTag(),tag)) // TODO: Unnecessary strcmp | Remove and Test
				return target;
		}
	}
	
	return nullptr;
}

Entity* GridSystem::findClosestEntity(Entity* ent, const std::vector<Entity*> &exceptions, int radius)
{
    int startingIndex = GetCellIndexFromEntity(ent);

    for (int i = 1; i <= radius-1; ++i)
    {
        auto target = SpiralSearch(ent, exceptions, startingIndex, i);
        if (target)
            return target;
    }

    return nullptr;
}

Entity* GridSystem::findClosestEntity(Entity* ent, const char* tag, const std::vector<Entity*> &exceptions, int radius)
{
    int startingIndex = GetCellIndexFromEntity(ent);

    for (int i = 1; i <= radius-1; ++i)
    {
        auto target = SpiralSearch(ent, tag, exceptions, startingIndex, i);
        if (target)
            return target;
    }

    return nullptr;
}

std::vector<Entity*> GridSystem::getAllWithTag(const char* tag)
{
    std::vector<Entity*> entities;
    for (auto e : _allEntities)
        if (!strcmp(e->getTag(),tag)) entities.emplace_back(e); // If Entity has same tag, add to vector
    return entities;
}

Entity* GridSystem::SpiralSearch(Entity* ent, int startingIndex, int depth)
{
    Entity* returnVal = nullptr;
	const int grid_boundary = _gridWidth * _gridHeight;

    int curr = startingIndex;
    int doubleDepth = depth*2;

    for (int i = 0; i < depth; ++i)
        curr = AboveIndex(curr);
	if (curr >= grid_boundary || curr < 0)
		returnVal = nullptr;
	else
	    returnVal = CheckForCandidate(ent, curr);
    if (returnVal) return returnVal;
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr); // Travel Index
		if (curr >= grid_boundary || curr < 0) // Check if Index Goes Out of Bounds
		{
			returnVal = nullptr; // Set Found Entity to nullptr
			continue; // Skip Candidate Checks | There are no Candidates | Continue instead of Break to keep continuity of Spiral Search
		}
		else
	        returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = BottomIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = LeftIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = AboveIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, curr);
        if (returnVal) return returnVal;
    }

    return returnVal;
}

Entity* GridSystem::SpiralSearch(Entity* ent, const char* tag, int startingIndex, int depth)
{
	Entity* returnVal = nullptr;
	const int grid_boundary = _gridWidth * _gridHeight;
	
	int curr = startingIndex;
	int doubleDepth = depth*2;
	
	for (int i = 0; i < depth; ++i)
		curr = AboveIndex(curr);
    if (curr >= grid_boundary || curr < 0)
        returnVal           = nullptr;
    else
        returnVal = CheckForCandidate(ent, tag, curr);
	if (returnVal) return returnVal;
	for (int i = 0; i < depth; ++i)
	{
		curr = RightIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, tag, curr);
		if (returnVal) return returnVal;
	}
	for (int i = 0; i < doubleDepth; ++i)
	{
		curr = BottomIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, tag, curr);
		if (returnVal) return returnVal;
	}
	for (int i = 0; i < doubleDepth; ++i)
	{
		curr = LeftIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, tag, curr);
		if (returnVal) return returnVal;
	}
	for (int i = 0; i < doubleDepth; ++i)
	{
		curr = AboveIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, tag, curr);
		if (returnVal) return returnVal;
	}
	for (int i = 0; i < depth; ++i)
	{
		curr = RightIndex(curr);
		if (curr >= grid_boundary || curr < 0)
		{
			returnVal = nullptr;
			continue;
		}
		else
			returnVal = CheckForCandidate(ent, tag, curr);
		if (returnVal) return returnVal;
	}
	
	return returnVal;
}

// SpiralSearch considering exceptions
Entity* GridSystem::SpiralSearch(Entity* ent, const std::vector<Entity*> &exceptions, int startingIndex, int depth)
{
    Entity* returnVal = nullptr;
    const int grid_boundary = _gridWidth * _gridHeight;

    int curr = startingIndex;
    int doubleDepth = depth*2;

    for (int i = 0; i < depth; ++i)
        curr = AboveIndex(curr);
    if (curr >= grid_boundary || curr < 0)
        returnVal           = nullptr;
    else
        returnVal = CheckForCandidate(ent, exceptions, curr);
    if (returnVal) return returnVal;
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr); // Travel Index
        if (curr >= grid_boundary || curr < 0) // Check if Index Goes Out of Bounds
        {
            returnVal = nullptr; // Set Found Entity to nullptr
            continue; // Skip Candidate Checks | There are no Candidates | Continue instead of Break to keep continuity of Spiral Search
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = BottomIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = LeftIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = AboveIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, curr);
        if (returnVal) return returnVal;
    }

    return returnVal;
}

// SpiralSearch considering tag and exceptions
Entity* GridSystem::SpiralSearch(Entity* ent, const char* tag, const std::vector<Entity*> &exceptions, int startingIndex, int depth)
{
    Entity* returnVal = nullptr;
    const int grid_boundary = _gridWidth * _gridHeight;

    int curr = startingIndex;
    int doubleDepth = depth*2;

    for (int i = 0; i < depth; ++i)
        curr = AboveIndex(curr);
    if (curr >= grid_boundary || curr < 0)
        returnVal           = nullptr;
    else
        returnVal = CheckForCandidate(ent, exceptions, tag, curr);
    if (returnVal) return returnVal;
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, tag, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = BottomIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, tag, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = LeftIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, tag, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = AboveIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, tag, curr);
        if (returnVal) return returnVal;
    }
    for (int i = 0; i < depth; ++i)
    {
        curr = RightIndex(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            returnVal = nullptr;
            continue;
        }
        else
            returnVal = CheckForCandidate(ent, exceptions, tag, curr);
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

Entity* GridSystem::CheckForCandidate(Entity* ent, const char* tag, int index)
{
	auto& cell = _grid[index].entities;
	if (!cell.empty())
		for (auto e : cell)
		{ if (ent != e && !strcmp(e->getTag(),tag)) return e; }
	return nullptr;
}

Entity* GridSystem::CheckForCandidate(Entity* ent, const std::vector<Entity*> &exceptions, int index)
{
    auto& cell = _grid[index].entities;
    if (!cell.empty())
        for (auto e : cell)
        {
            if (e == ent) continue;
            bool cleanEntity = true;
            for (auto ex : exceptions)
            {
                if (e == ex) cleanEntity = false;
            }
            if (cleanEntity) return e;
        }
    return nullptr;
}

Entity* GridSystem::CheckForCandidate(Entity* ent, const std::vector<Entity*> &exceptions, const char* tag, int index)
{
    auto& cell = _grid[index].entities;
    if (!cell.empty())
        for (auto e : cell)
        {
            if (e == ent) continue; // Prevent self return
            bool cleanEntity = true;
            for (auto ex : exceptions) // Prevent excepted Entities
            {
                if (e == ex)
                {
                    cleanEntity = false;
                    break;
                }
            }
            if (!strcmp(e->getTag(),tag) && cleanEntity) return e; // Tag Check and Return
        }
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
