#ifndef DX1220_SCENEA1_HPP
#define DX1220_SCENEA1_HPP

#include "SceneBase.h"
#include "MatrixStack.h"

#include "Entities/entity.hpp"

class SceneA1 : public SceneBase
{
	struct TileData
	{
        std::vector<Entity*> entitiesInTile;

	};

	const static int _gridXSize = 30;
	const static int _gridYSize = 30;
	float _gridWidth;
	float _gridHeight;
	Mesh* _whiteSquareMesh;
    Mesh* _blackSquareMesh;

	Mtx44 _projectionMatrix;
	Mtx44 _viewMatrix;
	Vector3 _tileSize;

    TileData _gridData[_gridXSize * _gridYSize];
	std::vector<GameObject*> _leftTeam;
	std::vector<GameObject*> _rightTeam;

	void MoveCamera(const Vector3& offset);
	
	void RenderEntity(Entity entity);
	
public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};


#endif
