#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <memory>
#include <string>

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
	virtual void Init();
	virtual void Update(const float& fDelta);
	virtual void Render();
	virtual void Run();
	virtual void Quit();

public:
	std::wstring GetLatestWinPixGpuCapturerPath();

public:
	inline LRESULT WINAPI AppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return ::DefWindowProc(hWnd, msg, wParam, lParam); }
};

