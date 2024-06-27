#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <memory>

class GraphicsPipeline;
class FrontEnd;

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
	std::unique_ptr<GraphicsPipeline>	GraphicsPipelineInstance;
	std::unique_ptr<FrontEnd>			FrontEndInstance;

protected:
	UINT Width;
	UINT Height;

public:
	void Init();
	void Run();

protected:
	void Render();

public:
	void Quit();

public:
	LRESULT WINAPI AppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

