#pragma once
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include "DefineUtility.h"

struct ID3D12Debug;

struct ID3D12Device;
struct IDXGISwapChain3;

struct ID3D12Resource;
struct ID3D12DescriptorHeap;

struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;


// TODO : 구조에 맞게 변경
#include <d3d12.h>
#include <memory>
struct ID3D12Resource;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Fence;

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

public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;

protected:
	float AspectRatio = 0.f;
	NotPublicVariable(AspectRatio);

public:
	void LoadPipeline(const UINT& Width, const UINT& Height);

	// TODO : 구조에 맞게 변경
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBuffer;
	std::unique_ptr<D3D12_VERTEX_BUFFER_VIEW> VertexBufferView;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	UINT FrameIndex;
	HANDLE FenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
	UINT64 FenceValue;

	void WaitForPreviousFrame();
	void LoadAsset();
	void OnRender();
	void PopulateCommandList();
};

