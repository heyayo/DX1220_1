#include "scenea1.hpp"

#include "Application.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"

#include "GL/glew.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z

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
    _meleeUnit = MeshBuilder::GenerateQuad(
            "MeleeUnit",
            {1,1,1}
            );
    _meleeUnit->textureID = LoadTGA("Image/meleeunit.tga");

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

    if (Application::IsMouseJustPressed(0))
    {
        double mouseX = 0,mouseY = 0;
        Application::GetCursorPos(&mouseX,&mouseY);
        auto worldCoords = ScreenToWorldSpace(mouseX,mouseY);
        if (worldCoords.first < 0 || worldCoords.first >= _gridWidth)
            std::cout << "Out of Bounds X Coordinates" << std::endl;
        if (worldCoords.second < 0 || worldCoords.second >= _gridHeight)
            std::cout << "Out of Bounds Y Coordinates" << std::endl;
        std::cout << worldCoords.first << ',' << worldCoords.second << std::endl;
    }
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
    delete _whiteSquareMesh;
    delete _blackSquareMesh;
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
    return {x + camera.position.x, y - camera.position.y};
}
