#include "PortfolioApp.h"
#include "GlobalVariable.h"
#include "GraphicsPipeline.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

#include <iostream>
#include <filesystem>
#include <shlobj.h>

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

	if (GetMonitorInfoA(Monitor, &MonitorInfo))
	{
		Width = MonitorInfo.rcWork.right - MonitorInfo.rcWork.left;
		Height = MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top;
	}
	else
	{
		cerr << "Getting Monitor Information Failed" << endl;
		exit(MONITOR_INFORMATION_ERROR);
	}

	SetWindowPos(MainWindow, NULL, 0, 0, Width, Height, SWP_SHOWWINDOW | SWP_NOMOVE);
	UpdateWindow(MainWindow);


	// TODO : 구조에 맞게 변경
	if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
	{
		LoadLibrary(GetLatestWinPixGpuCapturerPath().c_str());
	}

	GraphicsPipelineInstance = make_unique<GraphicsPipeline>();
	GraphicsPipelineInstance->LoadPipeline(Width, Height);

	// TODO : 구조에 맞게 변경
	if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
	{
		LoadLibrary(GetLatestWinPixGpuCapturerPath().c_str());
	}

	GraphicsPipelineInstance->LoadAsset();

	GraphicsPipelineInstance->m_viewport.Width = Width;
	GraphicsPipelineInstance->m_viewport.Height = Height;
	GraphicsPipelineInstance->m_viewport.MinDepth = 0.0f;
	GraphicsPipelineInstance->m_viewport.MaxDepth = 1.0f;

	GraphicsPipelineInstance->m_scissorRect.right = Width;
	GraphicsPipelineInstance->m_scissorRect.bottom = Height;
	while (true)
	{
		GraphicsPipelineInstance->OnRender();
	}
}

PortfolioApp::~PortfolioApp()
{
	DestroyWindow(MainWindow);
	UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
}

void PortfolioApp::Init()
{

}

void PortfolioApp::Update(const float& fDelta)
{
}

void PortfolioApp::Render()
{
}

void PortfolioApp::Run()
{
}

void PortfolioApp::Quit()
{
}

std::wstring PortfolioApp::GetLatestWinPixGpuCapturerPath()
{
	LPWSTR programFilesPath = nullptr;
	SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);
	filesystem::path pixInstallationPath = programFilesPath;
	pixInstallationPath /= "Microsoft PIX";

	std::wstring newestVersionFound;

	for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
	{
		if (directory_entry.is_directory())
		{
			if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
			{
				newestVersionFound = directory_entry.path().filename().c_str();
			}
		}
	}

	if (newestVersionFound.empty())
	{
		// TODO: Error, no PIX installation found
	}

	return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}
