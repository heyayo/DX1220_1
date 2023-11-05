#include "SceneA1TakeTwo.hpp"

#include "Application.h"
#include "MeshBuilder.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

//GridSystem SceneA1TakeTwo::_leftTeamGrid;
//GridSystem SceneA1TakeTwo::_rightTeamGrid;
GridSystem SceneA1TakeTwo::AllGrid;

void SceneA1TakeTwo::Init()
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
	_normalSquareMesh = MeshBuilder::GenerateQuad(
            "MeleeUnit",
            {1,1,1}
    );
	_birdTex = LoadImage("Image/berd.jpg");

    // Initialize Grid System
    float cellUniform = Application::GetWindowHeight()/30;
//    _leftTeamGrid.init(30, 30, cellUniform, cellUniform);
//    _rightTeamGrid.init(30, 30, cellUniform, cellUniform);
    AllGrid.init(30, 30, cellUniform, cellUniform);
	
	_testEnt1 = AllGrid.spawnEntity(_normalSquareMesh, _birdTex, {20, 20, 2});
	_testEnt1->setScale({50,50,50});
	_testEnt2 = AllGrid.spawnEntity(_normalSquareMesh, _birdTex, {20, 200, 2});
	_testEnt2->setScale({50,50,50});
	_testEnt3 = AllGrid.spawnEntity(_normalSquareMesh, _birdTex, {200, 20, 2});
	_testEnt3->setScale({50,50,50});
}

void SceneA1TakeTwo::Update(double deltaTime)
{
	static int radius = 1;
    float camSpeed = static_cast<float>(deltaTime) * 100.f;
    if (Application::IsKeyPressed(GLFW_KEY_A))
        MoveCamera({-camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_D))
        MoveCamera({camSpeed,0,0});
    if (Application::IsKeyPressed(GLFW_KEY_S))
        MoveCamera({0,-camSpeed,0});
    if (Application::IsKeyPressed(GLFW_KEY_W))
        MoveCamera({0,camSpeed,0});

	if (Application::IsMouseJustPressed(1))
	{
		Entity* result = AllGrid.findClosestEntity(_testEnt1,radius);
		if (result)
		{
			std::cout << result->getMesh()->name << std::endl;
			PrintVector(result->getPosition());
		}
		else std::cout << "No Find" << std::endl;
		std::cout << "Radius: " << radius << std::endl;
	}
	if (Application::IsMousePressed(0))
	{
		auto pos = MousePosWorldSpace();
		AllGrid.moveEntityAlongGrid(_testEnt1,
									{static_cast<float>(pos.first),static_cast<float>(pos.second),2},
									25*deltaTime);
	}
	if (Application::IsKeyPressed(GLFW_KEY_UP))
		++radius;
	if (Application::IsKeyPressed(GLFW_KEY_DOWN))
		--radius;
	/*
    for (auto& fsm : _fsms)
        fsm.Update(deltaTime);
	 */
}

void SceneA1TakeTwo::Render()
{
    SceneBase::Render();
    RenderGrid();
	RenderEntities();
}

void SceneA1TakeTwo::RenderGrid()
{
    auto gridSize = AllGrid.getGridSize();
    auto cellSize = AllGrid.getCellSize();
    modelStack.PushMatrix();

    modelStack.Scale(cellSize.first, cellSize.second,1);
    modelStack.Translate(0.5f,0.5f,0);
    Mesh* meshes[] = {_whiteSquareMesh,_blackSquareMesh};
    for (int i = 0; i < gridSize.first; ++i)
    {
        for (int j = 0; j < gridSize.second; ++j)
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

void SceneA1TakeTwo::RenderEntities()
{
	for (auto e : AllGrid.getAllEntities())
	{
		modelStack.PushMatrix();
		modelStack.Translate(UnpackVector(e->getPosition()));
		modelStack.Scale(UnpackVector(e->getScale()));
		e->getMesh()->textureID = e->getTextureID();
		RenderMesh(e->getMesh(),false);
		modelStack.PopMatrix();
	}
}

void SceneA1TakeTwo::Exit()
{
    SceneBase::Exit();

    delete _whiteSquareMesh;
    delete _blackSquareMesh;
    delete _normalSquareMesh;
}

void SceneA1TakeTwo::MoveCamera(const Vector3& offset)
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned SceneA1TakeTwo::LoadImage(const char* filepath)
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

template<typename T>
void SceneA1TakeTwo::SpawnEntityAt(unsigned int tex, const Vector3 &pos, GridSystem &team)
{
    auto ent = team.spawnEntity(_normalSquareMesh,tex);
    T* fsm = new T(ent);
	// TODO FINISH
}

void SceneA1TakeTwo::KillEntity()
{

}

std::pair<double, double> SceneA1TakeTwo::MousePos()
{
	double x,y;
	Application::GetCursorPos(&x,&y);
	y = Application::GetWindowHeight() - y;
	return {x,y};
}

std::pair<double, double> SceneA1TakeTwo::ScreenToWorldSpace(double x, double y)
{
	return {x + camera.position.x, y + camera.position.y};
}

std::pair<double, double> SceneA1TakeTwo::MousePosWorldSpace()
{
	auto temp = MousePos();
	return ScreenToWorldSpace(temp.first,temp.second);
}
