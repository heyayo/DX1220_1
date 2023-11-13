#include "SceneA1TakeTwo.hpp"

#include "Application.h"
#include "MeshBuilder.h"
#include "messager.hpp"

#include "FSMs/birdai.hpp"
#include "FSMs/bunnyai.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <algorithm>
#include <sstream>

#define UnpackVector(vec) vec.x,vec.y,vec.z
#define PrintVector(vec) std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

//GridSystem SceneA1TakeTwo::_leftTeamGrid;
//GridSystem SceneA1TakeTwo::_rightTeamGrid;
GridSystem SceneA1TakeTwo::AllGrid;
std::vector<InfoMsgRenderData> SceneA1TakeTwo::texts;
std::vector<StateMachine*> SceneA1TakeTwo::sms;

void SceneA1TakeTwo::Init()
{
    SceneBase::Init();
	
	staticWidth = Application::GetWindowWidth();
	staticHeight = Application::GetWindowHeight();

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
	_bunnyTex = LoadImage("Image/bunny.jpg");
    _treeTex = LoadImage("Image/tree.jpg");

    // Initialize Grid System
    float cellUniform = static_cast<float>(Application::GetWindowHeight())/30;
//    _leftTeamGrid.init(30, 30, cellUniform, cellUniform);
//    _rightTeamGrid.init(30, 30, cellUniform, cellUniform);
    AllGrid.init(30, 30, cellUniform, cellUniform);
	
	_presetTrees[0] = AllGrid.spawnEntity(_normalSquareMesh, _treeTex, {120, 120, 2});
	_presetTrees[1] = AllGrid.spawnEntity(_normalSquareMesh, _treeTex, {120, 480, 2});
	_presetTrees[2] = AllGrid.spawnEntity(_normalSquareMesh, _treeTex, {480, 480, 2});
	_presetTrees[3] = AllGrid.spawnEntity(_normalSquareMesh, _treeTex, {480, 120, 2});
    for (auto presetTree : _presetTrees)
	{
		presetTree->setRotation({0, 0, 180});
		presetTree->setTag("Tree");
	}

    auto trees = AllGrid.getAllWithTag("Tree");
    std::cout << "Fetched Trees | Size: " << trees.size() << std::endl;
	
	Messager::GetInstance().Register("scene", nullptr);
}

void SceneA1TakeTwo::Update(double deltaTime)
{
	// Debug Camera Movement
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
//		Entity* result = AllGrid.findClosestEntity(_testEnt1,"Berd",{_testEnt2,_testEnt3},radius);
//		if (result)
//		{
//			std::cout << result->getMesh()->name << std::endl;
//			PrintVector(result->getPosition());
//		}
//		else std::cout << "No Find" << std::endl;
//		std::cout << "Radius: " << radius << std::endl;
		auto pos = MousePosWorldSpace();
		std::cout << pos.first << ',' << pos.second << std::endl;
	}
	if (Application::IsMouseJustPressed(0))
	{
//        Entity* result = AllGrid.findClosestEntity(_testEnt1,"Berd",{_testEnt3},radius);
//        if (result)
//        {
//            std::cout << result->getMesh()->name << std::endl;
//            PrintVector(result->getPosition());
//        }
//        else std::cout << "No Find" << std::endl;
//        std::cout << "Radius: " << radius << std::endl;
//		auto pos = MousePosWorldSpace();
//		AllGrid.moveEntityAlongGrid(_testEnt1,
//									{static_cast<float>(pos.first),static_cast<float>(pos.second),2},
//									25*deltaTime);
		auto pos = MousePosWorldSpace();
		auto ent = AllGrid.spawnEntity(_normalSquareMesh,_bunnyTex,
	   {static_cast<float>(pos.first),static_cast<float>(pos.second),3});
		ent->setScale({20,20,20});
		ent->setTag("bunnies");
		std::vector<Entity*> tempTrees(4);
		tempTrees.assign(_presetTrees,_presetTrees+4);
		AttachAIToEntity<BunnyAI>(ent);
	}
	
	for (auto sm : sms)
	{
		sm->Update(deltaTime);
		sm->RenderTexts();
	}
	
	Account sceneMessages;
	// Not Error checking because the scene should always have an account
	Messager::GetInstance().FetchMessages("scene",sceneMessages);
	
	while (!sceneMessages.empty())
	{
		sceneMessages.front()->Handle();
		sceneMessages.pop();
	}
}

void SceneA1TakeTwo::Render()
{
    SceneBase::Render();
    RenderGrid();
	RenderEntities();
	for (auto data : texts)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],data.text,{0,1,0},data.size,data.loc.first,data.loc.second);
	}
	texts.clear();
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
		auto rot = e->getRotation();
		modelStack.Rotate(rot.z,0,0,1);
		modelStack.Scale(UnpackVector(e->getScale()));
		e->getMesh()->textureID = e->getTextureID();
		RenderMesh(e->getMesh(),false);
        e->getMesh()->textureID = 0;
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

template<typename T, typename... ARGS>
void SceneA1TakeTwo::AttachAIToEntity(Entity* ent, ARGS... a)
{
    T* machine = new T(ent, a...);
    sms.push_back(machine);
}

void SceneA1TakeTwo::KillAI(StateMachine* machine)
{
    // Find StateMachine in Structure
    auto iter = std::find(sms.begin(), sms.end(), machine);
    sms.erase(iter);

    // Kill Entity and Free StateMachine Memory
    AllGrid.despawnEntity(machine->getOwner());
    delete machine;
}

void SceneA1TakeTwo::KillAI(Entity* ent)
{
    // Search All AIs if they are owned by an Entity
    auto searchTerm = [&](StateMachine* i){ return i->getOwner() == ent; };
    auto iter = std::find_if(sms.begin(), sms.end(), searchTerm);

    // Remove StateMachine
    delete (*iter);
    sms.erase(iter);

    // Kill Entity
    AllGrid.despawnEntity(ent);
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
	// Scale Mouse Position to Current Window Size
	x = (x/Application::GetWindowWidth())*staticWidth;
	y = (y/Application::GetWindowHeight())*staticHeight;
	
	// Match Camera Position
	return {x + camera.position.x, y + camera.position.y};
}

std::pair<double, double> SceneA1TakeTwo::MousePosWorldSpace()
{
	auto temp = MousePos();
	return ScreenToWorldSpace(temp.first,temp.second);
}
