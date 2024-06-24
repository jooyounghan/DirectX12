#include "Texture2DObject.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"
#include "GraphicsPipeline.h"

#include "directx/d3dx12.h"

Texture2DObject::Texture2DObject(
	const UINT64 WidthIn,
	const UINT64 HeightIn,
	const UINT& ArraySizeIn,
	const UINT& MipLevelIn,
	DXGI_FORMAT FormatIn,
	bool IsMultiSampled,
	D3D12_RESOURCE_FLAGS ResourceFlagsIn
)
{
	MipLevel = MipLevelIn;
	Format = FormatIn;
	SetDimension(ArraySizeIn, IsMultiSampled);

	D3D12_RESOURCE_DESC TextureDesc;
	AutoZeroMemory(TextureDesc);

	TextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TextureDesc.Alignment = 0;
	TextureDesc.Width = WidthIn;
	TextureDesc.Height = HeightIn;
	TextureDesc.DepthOrArraySize = ArraySizeIn;
	TextureDesc.MipLevels = static_cast<UINT>(MipLevelIn);
	TextureDesc.Format = FormatIn;

	if (IsMultiSampled)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MultiSamplingFeature;
		ExitIfFailed(CHECK_FEATURE_SUPPORT_FAILED, GraphicsPipeline::GPipeline->Device->CheckFeatureSupport(
			D3D12_FEATURE::D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MultiSamplingFeature, sizeof(MultiSamplingFeature))
		);

		TextureDesc.SampleDesc.Count = MultiSamplingFeature.SampleCount < 4 ? 1 : 4;
		TextureDesc.SampleDesc.Quality = MultiSamplingFeature.NumQualityLevels > 0 ? MultiSamplingFeature.NumQualityLevels : MultiSamplingFeature.NumQualityLevels - 1;
	}
	else
	{
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
	}

	TextureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TextureDesc.Flags = ResourceFlagsIn;

	D3D12_CLEAR_VALUE ClearValue;
	AutoZeroMemory(ClearValue);
	ClearValue.Color[0] = 0.f;
	ClearValue.Color[1] = 0.f;
	ClearValue.Color[2] = 0.f;
	ClearValue.Color[3] = 1.f;

	CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
	ExitIfFailed(CREATE_COMMITTED_RESOURCE_FAILED, GraphicsPipeline::GPipeline->Device->CreateCommittedResource(
		&HeapProps,
		D3D12_HEAP_FLAG_NONE,
		&TextureDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(Texture2DResource.GetAddressOf()))
	);
}

Texture2DObject::~Texture2DObject()
{
}

void Texture2DObject::SetDimension(const UINT& ArraySizeIn, bool IsMultiSampled)
{
	if (ArraySizeIn > 1 && IsMultiSampled)
	{
		SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
		RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY;
	}
	else if (ArraySizeIn > 1)
	{
		SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	}
	else if (IsMultiSampled)
	{
		SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}
}
