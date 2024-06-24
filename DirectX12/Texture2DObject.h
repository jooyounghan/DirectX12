#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>

class Texture2DObject
{
public:
	Texture2DObject(
		const UINT64 WidthIn, 
		const UINT64 HeightIn,
		const UINT& ArraySizeIn,
		const UINT& MipLevelIn,
		DXGI_FORMAT FormatIn,
		bool IsMultiSampled,
		D3D12_RESOURCE_FLAGS ResourceFlagsIn
	);
	~Texture2DObject();

protected:
	UINT MipLevel;
	DXGI_FORMAT Format;
	D3D12_SRV_DIMENSION SRVDimension;
	D3D12_RTV_DIMENSION RTVDimension;
	D3D12_UAV_DIMENSION UAVDimension;

private:
	void SetDimension(const UINT& ArraySizeIn, bool IsMultiSampled);

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> Texture2DResource;

public:
	inline ID3D12Resource* GetResource() { return Texture2DResource.Get(); }
};

