#ifndef DX1220_SCENEA1_HPP
#define DX1220_SCENEA1_HPP

#include "SceneBase.h"
#include "MatrixStack.h"

class SceneA1 : public SceneBase
{
	struct Entity
	{
		Mesh* toRender;

	};

	struct TileData
	{
        std::vector<Entity*> entitiesInTile;

	};

	const static int _gridXSize = 50;
	const static int _gridYSize = 50;
	Mesh* _whiteSquareMesh;
    Mesh* _blackSquareMesh;

	Mtx44 _projectionMatrix;
	Mtx44 _viewMatrix;
	Vector3 _tileSize;

    TileData _gridData[_gridXSize * _gridYSize];

	void MoveCamera(const Vector3& offset);
	
public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};


#endif
