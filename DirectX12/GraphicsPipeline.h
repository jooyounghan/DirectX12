#pragma once

#include "HeaderHelper.h"

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

#include<memory>

class Texture2DObject;
class SRVObject;
class RTVObject;

class GraphicsPipeline
{
public:
	GraphicsPipeline();
	~GraphicsPipeline();
	
public:
	static GraphicsPipeline* GPipeline;

public:
	Microsoft::WRL::ComPtr<ID3D12Debug> DebugController;

protected:
	static const UINT BackBufferCount = 2;
	UINT FrameIndex = 0;
	MakeSetterGetter(FrameIndex);

public:
	Microsoft::WRL::ComPtr<ID3D12Device> Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChain;
	
public:
	std::unique_ptr<Texture2DObject>	BackBufferTextures[BackBufferCount];
	std::unique_ptr<RTVObject>			BackBufferRTVs;

public:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue;
	const enum D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

public:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;

protected:
	HANDLE FenceEvent = NULL;
	Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue = NULL;

protected:
	float AspectRatio = 0.f;
	MakeSetterGetter(AspectRatio);

protected:
	std::wstring GetLatestWinPixGpuCapturerPath();

public:
	void LoadPipeline(const UINT& WidthIn, const UINT& HeightIn);

public:
	void PrepareRender();
	void ExecuteRender();

public:
	void WaitForPreviousFrame();
};

