#include "PortfolioApp.h"
#include "GlobalVariable.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

#include "GraphicsPipeline.h"
#include "GameWorld.h"

#ifdef _DEBUG
#include "EditorWorld.h"
#endif 


using namespace std;

PortfolioApp* PortfolioApp::GApp = nullptr;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return PortfolioApp::GApp->AppProc(hWnd, msg, wParam, lParam);
}

PortfolioApp::PortfolioApp()
{
	GApp = this;

	AutoZeroMemory(WindowClass);
	WindowClass = {
		sizeof(WNDCLASSEX), CS_CLASSDC, WndProc,
		0L, 0L, GetModuleHandle(NULL), 
		NULL, NULL, NULL, 
		NULL, L"Portfolio", NULL 
	};
	RegisterClassEx(&WindowClass);

	HWND hWindow = CreateWindow(
		WindowClass.lpszClassName, L"Joo YH / Portfolio_DirectX11",
		WS_OVERLAPPEDWINDOW, 0, 0,
		NULL, NULL, NULL, 
		NULL, WindowClass.hInstance, NULL
	);

	MainWindow = hWindow;
	Graphics::GMainWindow = MainWindow;

	HMONITOR Monitor = MonitorFromWindow(MainWindow, MONITOR_DEFAULTTONEAREST);
	MONITORINFO MonitorInfo;
	AutoZeroMemory(MonitorInfo);
	MonitorInfo.cbSize = sizeof(MONITORINFO);

	ExitIfFailed(MONITOR_INFORMATION_ERROR, GetMonitorInfoA(Monitor, &MonitorInfo));

	Width = MonitorInfo.rcWork.right - MonitorInfo.rcWork.left;
	Height = MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top;

	Graphics::GWidth = Width;
	Graphics::GHeight = Height;

	SetWindowPos(MainWindow, NULL, 0, 0, Width, Height, SWP_SHOWWINDOW | SWP_NOMOVE);
	UpdateWindow(MainWindow);
}

PortfolioApp::~PortfolioApp()
{
	DestroyWindow(MainWindow);
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}

void PortfolioApp::Init()
{
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&PrevTime);

	GraphicsPipelineInstance = make_unique<GraphicsPipeline>();
	GraphicsPipelineInstance->LoadPipeline(Width, Height);

	GameWorldInstance = make_unique<GameWorld>(GraphicsPipelineInstance->Device.Get());
	GameWorldInstance->LoadGameWorld();

#ifdef _DEBUG
	EditorWorldInstance = make_unique<EditorWorld>(GraphicsPipelineInstance->Device.Get(), GameWorldInstance.get());
	EditorWorldInstance->Init(MainWindow);
#endif 
}

void PortfolioApp::Run()
{
	// Main message loop
	MSG msg = {};
	while (WM_QUIT != msg.message) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			Render();
		}
	}
}

void PortfolioApp::Quit()
{
}

void PortfolioApp::Render()
{
	const float& DeltaTime = GetDeltaTimeFromLastCall();
	const UINT& FrameIndex = GraphicsPipelineInstance->GetFrameIndex();

	GraphicsPipelineInstance->PrepareRender();

	GameWorldInstance->UpdateGameWorld(DeltaTime);

#ifdef _DEBUG
	EditorWorldInstance->DrawToBackBuffer(
		FrameIndex,
		GraphicsPipelineInstance->BackBufferTextures[FrameIndex].get(),
		GraphicsPipelineInstance->BackBufferRTVs.get(),
		GraphicsPipelineInstance->CommandList.Get()
	);
#elif
	GameWorld->DrawToBackBuffer(
		FrameIndex,
		GraphicsPipelineInstance->BackBufferTextures[FrameIndex].get(),
		GraphicsPipelineInstance->BackBufferRTVs.get(),
		GraphicsPipelineInstance->CommandList.Get()
	);
#endif

	GraphicsPipelineInstance->ExecuteRender();
}

float PortfolioApp::GetDeltaTimeFromLastCall()
{
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);

	LONGLONG Elapsed = CurrentTime.QuadPart - PrevTime.QuadPart;
	const float DeltaTime = static_cast<float>(Elapsed) / Frequency.QuadPart;

	PrevTime = CurrentTime;
	return DeltaTime;
}



LRESULT __stdcall PortfolioApp::AppProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (EditorWorldInstance.get() && EditorWorldInstance->EditorWorldProc(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) 
	{
	case WM_SIZE:
		return 0;
	case WM_EXITSIZEMOVE:
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
