#pragma once
#include "ResourceObject.h"
#include "HeaderHelper.h"

class Texture2DObject : public ResourceObject
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
	Texture2DObject(const Microsoft::WRL::ComPtr<ID3D12Resource>& ResourceIn);
	virtual ~Texture2DObject();

protected:
	UINT MipLevel;
	MakeGetter(MipLevel);
	DXGI_FORMAT Format;
	MakeGetter(Format);
	D3D12_SRV_DIMENSION SRVDimension;
	MakeGetter(SRVDimension);
	D3D12_RTV_DIMENSION RTVDimension;
	MakeGetter(RTVDimension);
	D3D12_UAV_DIMENSION UAVDimension;
	MakeGetter(UAVDimension);

private:
	void SetDimension(const UINT& ArraySizeIn, bool IsMultiSampled);

public:
	inline ID3D12Resource* GetResource() { return Resource.Get(); }
};

