#include "SceneTicTacToe.h"
#include "GL/glew.h"
#include "Application.h"
#include <sstream>

SceneTicTacToe::SceneTicTacToe()
{
}

SceneTicTacToe::~SceneTicTacToe()
{
}

void SceneTicTacToe::Init()
{
	SceneBase::Init();

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	m_objectCount = 0;
	m_noGrid = 3;
	m_gridSize = m_worldHeight / m_noGrid;
	m_gridOffset = m_gridSize / 2;

	m_bCrossTurn = true;
	m_bGameOver = false;
	m_winner = WHO_CONTENT::WHO_NONE;
	
	m_ghost = nullptr;
}

GameObject* SceneTicTacToe::FetchGO()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (!go->active)
		{
			go->active = true;
			++m_objectCount;
			return go;
		}
	}
	for (unsigned i = 0; i < 10; ++i)
	{
		GameObject *go = new GameObject(GameObject::GO_CROSS);
		m_goList.push_back(go);
	}
	return FetchGO();
}

//Exercise: This function should return 0 to 8, i.e. AI _player's decision to put circle on one of the grids
int SceneTicTacToe::GetAIDecision()
{
	return 0;
}

#include "GLFW/glfw3.h"

void SceneTicTacToe::Update(double dt)
{
	SceneBase::Update(dt);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	if (Application::IsKeyPressed(GLFW_KEY_MINUS))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed(GLFW_KEY_EQUAL))
	{
		m_speed += 0.1f;
	}
	if (Application::IsKeyPressed('R'))
	{
		for (auto& cell : m_grid)
            cell = WHO_NONE;

	}

	//Input Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float posX = static_cast<float>(x) / w;
		float posY = (h - static_cast<float>(y)) / h;
		
		float worldPosX = posX * m_worldWidth;
		float worldPosY = posY * m_worldHeight;
		
        if (posX <= 0.6)
        {
            int xIndex = (posX * 5);
            int yIndex = (posY * 3);
            if (m_bCrossTurn)
                m_grid[xIndex+(3*yIndex)] = WHO_CROSS;
            else m_grid[xIndex+(3*yIndex)] = WHO_CIRCLE;
            m_bCrossTurn = !m_bCrossTurn;
        }
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}
	static bool bSpaceState = false;
	if (!bSpaceState && Application::IsKeyPressed(GLFW_KEY_SPACE))
	{
		bSpaceState = true;
	}
	else if (bSpaceState && !Application::IsKeyPressed(GLFW_KEY_SPACE))
	{
		bSpaceState = false;
	}

	if (!m_bCrossTurn)
	{
		//Exercise: Call GetAIDecision() and put circle on grid
	}

	//Game Logic Section
	//Exercise: Check draw, cross wins or circle wins
}


void SceneTicTacToe::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_CROSS:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CROSS], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_CIRCLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CIRCLE], false);
		modelStack.PopMatrix();
		break;
	}
}

void SceneTicTacToe::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(m_worldHeight * 0.5f, m_worldHeight * 0.5f, -1.f);
	modelStack.Scale(m_worldHeight, m_worldHeight, m_worldHeight);
	RenderMesh(meshList[GEO_TICTACTOE], false);
	modelStack.PopMatrix();

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	//On screen text
	static std::string winnerText[] = {"Draw", "Cross wins", "Circle wins"};

	std::ostringstream ss;
	ss.precision(3);
	ss << "Speed:" << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 6);

	ss.str("");
	ss.precision(5);
	ss << "FPS:" << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 3);

	ss.str("");
	ss << "Count:" << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 9);

	ss.str("");
	ss << "Turn:" << (m_bCrossTurn ? "Cross" : "Circle");
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 12);

	if (m_bGameOver)
	{
		ss.str("");
		ss << winnerText[static_cast<int>(m_winner)];
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 50, 15);
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Tic Tac Toe (R to reset)", Color(0, 1, 0), 3, 50, 0);
}

void SceneTicTacToe::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}
