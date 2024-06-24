#pragma once

#include "HeaderHelper.h"

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <string>

#include "directx/d3dx12.h"

struct Point
{
		float x;
		float y;
		float z;
};

struct Color
{
	float r;
	float g;
	float b;
	float a;
};

struct Vertex
{
	Point PointValue;
	Color ColorValue;
};

class GraphicsPipeline
{
public:
	GraphicsPipeline();
	~GraphicsPipeline();
	
public:
	static GraphicsPipeline* GPipeline;

public:
	Microsoft::WRL::ComPtr<ID3D12Debug> DebugController;

public:
	static const UINT BackBufferCount = 2;

public:
	Microsoft::WRL::ComPtr<ID3D12Device> Device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChain;
	
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> RenderTargets[BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTVHeapDescriptor;
	UINT RTVDescriptorSize = 0;

public:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue;
	const enum D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;

protected:
	float AspectRatio = 0.f;
	MakeSetterGetter(AspectRatio);

protected:
	std::wstring GetLatestWinPixGpuCapturerPath();

public:
	void LoadPipeline(const UINT& WidthIn, const UINT& HeightIn);

	// TODO : 구조에 맞게 변경
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBuffer;
	std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> VertexBufferView;

	UINT FrameIndex;
	HANDLE FenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue;

	void WaitForPreviousFrame();
	void LoadAsset();
	void OnRender();
	void PopulateCommandList();
};

