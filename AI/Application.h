
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

#include <random>

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
    static bool IsMouseJustPressed(unsigned short key);
	static void GetCursorPos(double *xpos, double *ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static void* GetWindowPtr();
    static unsigned LoadImage(const char* filepath);
    static void* getWindow();

    static std::mt19937 randomthing;
    static double DELTATIME;

private:
	Application();
	~Application();

    static bool _mouseRecords[3];
    static bool _mouseCurrent[3];

	//Declare a window object
	StopWatch m_timer;

};

#endif