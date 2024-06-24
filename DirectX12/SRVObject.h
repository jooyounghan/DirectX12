#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include "directx/d3dx12.h"

class SRVObject
{
public:
	SRVObject(
		ID3D12Resource* pResource, 
		DXGI_FORMAT FormatIn,
		D3D12_SRV_DIMENSION ViewDimensionIn,
		UINT MipLevelsIn
	);
	~SRVObject();

public:
	D3D12_CPU_DESCRIPTOR_HANDLE SRVHandle;

};

