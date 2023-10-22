#include "scenea1.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "MatrixStack.h"

#include "GL/glew.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z

void SceneA1::Init()
{
	SceneBase::Init();
	
	// Initializing Projection Matrix
	projectionMatrix.SetToOrtho(
		0,Application::GetWindowWidth(),
		0,Application::GetWindowHeight(),
		-10,10
		);
	projectionStack.LoadIdentity();
	projectionStack.LoadMatrix(projectionMatrix);
	
	camera.Init({0,0,1},
				{0,0,0},
				{0,1,0});
	
	viewMatrix.SetToLookAt(
		UnpackVector(camera.position),
		UnpackVector(camera.target),
		UnpackVector(camera.up)
		);
	viewStack.LoadIdentity();
	viewStack.LoadMatrix(viewMatrix);
	
	squareMesh = MeshBuilder::GenerateQuad(
		"TileMesh",
		{1,1,1});
	
	tileSize =
		{
			static_cast<float>(Application::GetWindowWidth())
			/ static_cast<float>(_gridXSize),
			static_cast<float>(Application::GetWindowWidth())
			/ static_cast<float>(_gridYSize),
			1
		};
	
	std::cout << Application::GetWindowWidth() << ',' << Application::GetWindowHeight() << std::endl;
}

void SceneA1::Update(double dt)
{

}

void SceneA1::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	modelStack.LoadIdentity();
	
	modelStack.PushMatrix();
	modelStack.Translate(Application::GetWindowWidth()/2.f,
						 Application::GetWindowHeight()/2.f,
						 -1.f);
	modelStack.Scale(UnpackVector(tileSize));
	RenderMesh(squareMesh,false);
}

void SceneA1::Exit()
{
}

void SceneA1::MoveCamera(const Vector3& offset)
{
	// Change Camera Values
	camera.position += offset;
	camera.target += offset;
	
	// Recalculate based off new values
	viewMatrix.SetToLookAt(
		UnpackVector(camera.position),
		UnpackVector(camera.target),
		UnpackVector(camera.up)
	);
}
