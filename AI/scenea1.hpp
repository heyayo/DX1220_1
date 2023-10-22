#ifndef DX1220_SCENEA1_HPP
#define DX1220_SCENEA1_HPP

#include "SceneBase.h"
#include "MatrixStack.h"

class SceneA1 : public SceneBase
{
	const static int _gridXSize = 50;
	const static int _gridYSize = 50;
	Mesh* squareMesh;
	
	Mtx44 projectionMatrix;
	Mtx44 viewMatrix;
	Vector3 tileSize;
	
	struct Entity
	{
		Mesh* toRender;
		
	};
	
	struct TileData
	{
	
	};
	
	void MoveCamera(const Vector3& offset);
	
public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};


#endif
