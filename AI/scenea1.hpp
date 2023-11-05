#ifndef DX1220_SCENEA1_HPP
#define DX1220_SCENEA1_HPP

#include "SceneBase.h"
#include "MatrixStack.h"

#include "Entities/entity.hpp"

#include <queue>

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
    Mesh* _meleeUnitMesh;

	Mtx44 _projectionMatrix;
	Mtx44 _viewMatrix;
	static Vector3 _tileSize;

    static TileData _gridData[_gridXSize * _gridYSize];
	static std::vector<Entity*> _leftTeam;
	static std::vector<Entity*> _rightTeam;
    //static std::vector<FSM*> _fsms;

	void MoveCamera(const Vector3& offset);
	
	void RenderEntity(Entity* entity);
    std::pair<double,double> ScreenToWorldSpace(double x, double y);
	static std::pair<double,double> MousePos();
	std::pair<double,double> MousePosWorldSpace();
	
	static unsigned LoadImage(const char* filepath);
	
	void MouseSpawnEntity();
	void PhysicsStep();
	static int ConvertXYToIndex(float x, float y);
	
public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	
	enum class PhysEventType : unsigned char
	{
		MOVE = 0,
		ALL_TYPE = 255
	};
	
	struct PhysEvent
	{
		Entity* issuer;
		PhysEventType type;
		Vector3 velocity;
	};
	
	static std::queue<PhysEvent> physicsEventQueue;

    static Entity* BruteForceFindCloseEntities(const std::vector<Entity*> team, const Vector3& from);

    static Vector3 FindCloseEntities(int tileRange, const Vector3& from);
    static int SpiralSearch(int radius, int startingIndex);
    static int AboveIndex(int from);
    static int BottomIndex(int from);
    static int LeftIndex(int from);
    static int RightIndex(int from);

    static const std::vector<Entity*>& LeftTeam();
    static const std::vector<Entity*>& RightTeam();
};


#endif
