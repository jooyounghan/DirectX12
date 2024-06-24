#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <memory>

class GraphicsPipeline;

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
	std::unique_ptr<GraphicsPipeline> GraphicsPipelineInstance;

protected:
	UINT Width;
	UINT Height;

public:
	void Init();
	void Run();

protected:
	void Update(const float& fDelta);
	void Render();

public:
	void Quit();

public:
	inline LRESULT WINAPI AppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, msg, wParam, lParam); }
};

