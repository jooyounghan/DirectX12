#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>

class RTVObject
{
public:
	RTVObject(
		ID3D12Device* Device,
		UINT ResourceCount,
		ID3D12Resource** Resources
	);
	~RTVObject();

public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapDescriptor;
	UINT RTVDescriptorSize = 0;
};

