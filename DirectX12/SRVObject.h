#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>

class SRVObject
{
public:
	SRVObject(
		ID3D12Device* Device,
		UINT ResourceCount,
		ID3D12Resource** Resources
	);
	~SRVObject();

public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapDescriptor;
	UINT SRVDescriptorSize = 0;
};

