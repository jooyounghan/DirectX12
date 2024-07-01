#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <memory>

class GraphicsPipeline;
class EditorWorld;
class GameWorld;

class PortfolioApp
{
public:
	PortfolioApp();
	~PortfolioApp();

public:
	static PortfolioApp* GApp;

protected:
	WNDCLASSEX WindowClass;
	HWND MainWindow;

protected:
	LARGE_INTEGER Frequency = LARGE_INTEGER();
	LARGE_INTEGER PrevTime = LARGE_INTEGER();

protected:
	std::unique_ptr<GraphicsPipeline>	GraphicsPipelineInstance;
	std::unique_ptr<GameWorld>				GameWorldInstance;

#ifdef _DEBUG
	std::unique_ptr<EditorWorld>			EditorWorldInstance;
#endif 

protected:
	UINT Width = 1280;
	UINT Height = 960;

public:
	void Init();
	void Run();
	void Quit();

protected:
	void Render();

private:
	float GetDeltaTimeFromLastCall();

public:
	LRESULT WINAPI AppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

