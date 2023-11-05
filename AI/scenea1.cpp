#include "scenea1.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GL/glew.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z

Vector3 SceneA1::_tileSize;
std::vector<Entity*> SceneA1::_leftTeam;
std::vector<Entity*> SceneA1::_rightTeam;
//std::vector<FSM*> SceneA1::_fsms;
std::queue<SceneA1::PhysEvent> SceneA1::physicsEventQueue;
SceneA1::TileData SceneA1::_gridData[_gridXSize * _gridYSize];

void SceneA1::Init()
{
	SceneBase::Init();
	
	// Initializing Projection Matrix
	_projectionMatrix.SetToOrtho(
		0,Application::GetWindowWidth(),
		0,Application::GetWindowHeight(),
		-10,10
		);
	projectionStack.LoadIdentity();
	projectionStack.LoadMatrix(_projectionMatrix);
	
	camera.Init({0,0,1},
				{0,0,0},
				{0,1,0});
	
	_viewMatrix.SetToLookAt(
		UnpackVector(camera.position),
		UnpackVector(camera.target),
		UnpackVector(camera.up)
		);
	viewStack.LoadIdentity();
	viewStack.LoadMatrix(_viewMatrix);

    _whiteSquareMesh = MeshBuilder::GenerateQuad(
		"WhiteMesh",
		{1,1,1});
    _blackSquareMesh = MeshBuilder::GenerateQuad(
		"BlackMesh",
		{0,0,0});
    _meleeUnitMesh = MeshBuilder::GenerateQuad(
            "MeleeUnit",
            {1,1,1}
            );
    _meleeUnitMesh->textureID = LoadImage("Image/sword.png");

    _tileSize =
		{
			static_cast<float>(Application::GetWindowHeight())
			/ static_cast<float>(_gridXSize),
			static_cast<float>(Application::GetWindowHeight())
			/ static_cast<float>(_gridYSize),
			1
		};
	
	_gridWidth = _gridXSize * static_cast<float>(Application::GetWindowHeight())
							  / static_cast<float>(_gridXSize);
	_gridHeight = _gridYSize * static_cast<float>(Application::GetWindowHeight())
							   / static_cast<float>(_gridYSize);
}

int SceneA1::ConvertXYToIndex(float x, float y)
{
	int xIndex = static_cast<int>(x/_tileSize.x);
	int yIndex = static_cast<int>(y/_tileSize.y);
	return xIndex + (_gridYSize * yIndex);
}

#include "GLFW/glfw3.h"

void SceneA1::Update(double dt)
{
    float camSpeed = static_cast<float>(dt) * 100.f;
    if (Application::IsKeyPressed(GLFW_KEY_D))
        MoveCamera({-camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_A))
        MoveCamera({camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_W))
        MoveCamera({0,-camSpeed,0});
    if (Application::IsKeyPressed(GLFW_KEY_S))
        MoveCamera({0,camSpeed,0});

    MouseSpawnEntity();

//    for (auto& fsm : _fsms)
//    {
//        fsm->Update(dt);
//    }
}

void SceneA1::MouseSpawnEntity()
{
	if (Application::IsMouseJustPressed(0))
	{
		auto worldCoords = MousePosWorldSpace();
		// Bounds Checking
		if (worldCoords.first < 0 || worldCoords.first >= _gridWidth)
		{
			std::cout << "Out of Bounds X Coordinates" << std::endl;
			return;
		}
		if (worldCoords.second < 0 || worldCoords.second >= _gridHeight)
		{
			std::cout << "Out of Bounds Y Coordinates" << std::endl;
			return;
		}
		std::cout << worldCoords.first << ',' << worldCoords.second << std::endl;
		
		// Allocate Entity and Assign Team
//		Entity* temp = new Entity(_meleeUnitMesh);
//		temp->setPosition(
//			{static_cast<float>(worldCoords.first),
//			 static_cast<float>(worldCoords.second),2});
//		temp->setScale({50,50,50});
//
//		// Assign Left or Right Team based on Grid Location
//		if (worldCoords.first > _gridWidth*0.5f)
//        {
//            _leftTeam.emplace_back(temp);
//        }
//		else
//        {
//            _rightTeam.emplace_back(temp);
//        }

        /*
         * Not using Grid Physics Anymore
		// Put Entity Into Grid Cell
		int index = ConvertXYToIndex(worldCoords.first,worldCoords.second);
		std::cout << "Final Index: " << index << std::endl;
		_gridData[index].entitiesInTile.emplace_back(temp);
         */
	}
	
	if (Application::IsMouseJustPressed(1))
	{
		auto mousePos = MousePosWorldSpace();
        /*
		PhysEvent event{_leftTeam.front(),PhysEventType::MOVE,};
		_leftTeam.front()->setPosition(
			{static_cast<float>(mousePos.first),
			 static_cast<float>(mousePos.second), 2});
         */
	}
}

#include <algorithm>

// Unused
void SceneA1::PhysicsStep()
{
	auto event = physicsEventQueue.front();
	
	// MOVE Event
	if ((unsigned char)event.type & (unsigned char)PhysEventType::MOVE)
	{
		// Find Entity in Old Cell
		auto pos = event.issuer->getPosition();
		int oldIndex = ConvertXYToIndex(pos.x,pos.y);
		auto olditer = std::find(
			_gridData->entitiesInTile.begin(),
			_gridData->entitiesInTile.end(),
			event.issuer
			);
		// Remove from Old Cell
		_gridData->entitiesInTile.erase(olditer);
		// MOVE Entity
		event.issuer->setPosition(pos + event.velocity);
		// Find New Cell
		pos = event.issuer->getPosition();
		int newIndex = ConvertXYToIndex(pos.x,pos.y);
		// Add to New Cell
		_gridData[newIndex].entitiesInTile.emplace_back(event.issuer);
	}
	// Other Events
	
	physicsEventQueue.pop();
}

void SceneA1::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	modelStack.LoadIdentity();

    // Render Grid using Individual Black and White Squares
    {
        modelStack.PushMatrix();
        /*
            modelStack.Translate(Application::GetWindowWidth()/2.f,
                                 Application::GetWindowHeight()/2.f,
                                 -1.f);
         */
        modelStack.Scale(UnpackVector(_tileSize));
		// Offset Square "Render From Bottom Left Corner"
        modelStack.Translate(.5f,.5f,0);
        Mesh* meshes[] = {_whiteSquareMesh,_blackSquareMesh};
        for (int i = 0; i < _gridXSize; ++i)
        {
            for (int j = 0; j < _gridYSize; ++j)
            {
                modelStack.PushMatrix();
                modelStack.Translate(static_cast<float>(i),static_cast<float>(j),0);
                int tileIndex = (j%2) != (i%2);
                RenderMesh(meshes[tileIndex], false);
                modelStack.PopMatrix();
            }
        }
        modelStack.PopMatrix();
    }

    // Rendering Entities
    for (auto x : _leftTeam)
    {
        RenderEntity(x);
    }
    for (auto x : _rightTeam)
    {
        RenderEntity(x);
    }
}

void SceneA1::Exit()
{
	// Free Mesh Pointers
    delete _whiteSquareMesh;
    delete _blackSquareMesh;
	delete _meleeUnitMesh;
	
	// Free Entities and FSMs
	for (auto& x : _leftTeam)
		delete x;
	for (auto& x : _rightTeam)
		delete x;
//    for (auto& x : _fsms)
//        delete x;
}

void SceneA1::MoveCamera(const Vector3& offset)
{
	// Change Camera Values
	camera.position += offset;
	camera.target += offset;
	
	// Recalculate based off new values
	_viewMatrix.SetToLookAt(
		UnpackVector(camera.position),
		UnpackVector(camera.target),
		UnpackVector(camera.up)
	);
    viewStack.LoadMatrix(_viewMatrix);
}

void SceneA1::RenderEntity(Entity* entity)
{
	modelStack.PushMatrix();
	modelStack.Translate(UnpackVector(entity->getPosition()));
	modelStack.Scale(UnpackVector(entity->getScale()));
	RenderMesh(entity->getMesh(),false);
	modelStack.PopMatrix();
}

std::pair<double,double> SceneA1::ScreenToWorldSpace(double x, double y)
{
    return {x + camera.position.x, y + camera.position.y};
}

std::pair<double, double> SceneA1::MousePos()
{
	double x,y;
	Application::GetCursorPos(&x,&y);
	y = Application::GetWindowHeight() - y;
	return {x,y};
}

std::pair<double, double> SceneA1::MousePosWorldSpace()
{
	auto temp = MousePos();
	return ScreenToWorldSpace(temp.first,temp.second);
}

#include "stb_image.h"

unsigned SceneA1::LoadImage(const char* filepath)
{
	int x,y,n;
	unsigned char* pixel_data = stbi_load(filepath,&x,&y,&n,4);
	if (!pixel_data)
	{
		std::cerr << "Failed To Load Image | " << filepath << std::endl;
		std::cerr << stbi_failure_reason() << std::endl;
		return 0;
	}
	
	GLuint textureID = 0;
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,x,y,0,GL_BGRA,GL_UNSIGNED_BYTE,pixel_data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	float maxAnisotropy = 1.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLint)maxAnisotropy );
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glGenerateMipmap( GL_TEXTURE_2D );
	
	stbi_image_free(pixel_data);
	
	return textureID;
}

Vector3 SceneA1::FindCloseEntities(int tileRange, const Vector3 &from)
{
    int fromIndex = ConvertXYToIndex(from.x,from.y);

    // Spiral Search


    return {};
}

int SceneA1::AboveIndex(int from)
{
    return from+_gridYSize;
}

int SceneA1::BottomIndex(int from)
{
    return from-_gridYSize;
}

int SceneA1::LeftIndex(int from)
{
    return from-1;
}

int SceneA1::RightIndex(int from)
{
    return from+1;
}

int SceneA1::SpiralSearch(int radius, int startingIndex)
{
    // Entity TileCheck Lambda
    auto tileCheck = [](int index){ return !_gridData[index].entitiesInTile.empty(); };

    // Top to Right
    int now = startingIndex;
    for (int i = 0; i < radius; ++i)
        now = AboveIndex(now);
    if (tileCheck(now)) return now;
    for (int i = 0; i< radius; ++i)
    {
        now = RightIndex(now);
        if (tileCheck(now)) return now;
    }

    // Right to Down
    now = startingIndex;
    for (int i = 0; i < radius; ++i)
        now = RightIndex(now);
    if (tileCheck(now)) return now;
    for (int i = 0; i< radius; ++i)
    {
        now = BottomIndex(now);
        if (tileCheck(now)) return now;
    }

    // Down to Left
    now = startingIndex;
    for (int i = 0; i < radius; ++i)
        now = BottomIndex(now);
    if (tileCheck(now)) return now;
    for (int i = 0; i< radius; ++i)
    {
        now = LeftIndex(now);
        if (tileCheck(now)) return now;
    }

    // Left to Up
    now = startingIndex;
    for (int i = 0; i < radius; ++i)
        now = LeftIndex(now);
    if (tileCheck(now)) return now;
    for (int i = 0; i< radius; ++i)
    {
        now = AboveIndex(now);
        if (tileCheck(now)) return now;
    }
	
	return now;
}

Entity* SceneA1::BruteForceFindCloseEntities(const std::vector<Entity*> team, const Vector3& from)
{
    float shortestDistance = std::numeric_limits<float>::max();
    Entity* closestEntity = nullptr;
    for (auto& entity : team)
    {
        auto deltaPositions = entity->getPosition() - from;
        float deltaPositionLength = deltaPositions.LengthSquared();
        if (deltaPositionLength < shortestDistance)
        {
            shortestDistance = deltaPositionLength;
            closestEntity = entity;
        }
    }

    return closestEntity;
}

const std::vector<Entity*> &SceneA1::LeftTeam()
{
    return _leftTeam;
}

const std::vector<Entity*> &SceneA1::RightTeam()
{
    return _rightTeam;
}
