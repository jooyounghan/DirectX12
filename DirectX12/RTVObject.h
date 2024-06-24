#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include "directx/d3dx12.h"

class RTVObject
{
public:
	RTVObject(
		ID3D12Resource* pResource,
		DXGI_FORMAT FormatIn,
		D3D12_RTV_DIMENSION ViewDimensionIn
	);
	~RTVObject();

public:
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
};

