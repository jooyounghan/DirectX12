#include "Texture2DObject.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"
#include "GraphicsPipeline.h"
#include "d3dx12.h"

Texture2DObject::Texture2DObject(
	const UINT64 WidthIn,
	const UINT HeightIn,
	const UINT& ArraySizeIn,
	const UINT& MipLevelIn,
	DXGI_FORMAT FormatIn,
	bool IsMultiSampled,
	D3D12_RESOURCE_FLAGS ResourceFlagsIn
)
	: ResourceObject(D3D12_RESOURCE_STATE_COMMON)
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
		AutoZeroMemory(MultiSamplingFeature);

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
	ClearValue.Format = FormatIn;
	ClearValue.Color[0] = 0.f;
	ClearValue.Color[1] = 0.f;
	ClearValue.Color[2] = 0.f;
	ClearValue.Color[3] = 1.f;

	CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
	ExitIfFailed(CREATE_COMMITTED_RESOURCE_FAILED, GraphicsPipeline::GPipeline->Device->CreateCommittedResource(
		&HeapProps,
		D3D12_HEAP_FLAG_NONE,
		&TextureDesc,
		CurrentState,
		&ClearValue,
		IID_PPV_ARGS(Resource.GetAddressOf()))
	);
}

Texture2DObject::Texture2DObject(const Microsoft::WRL::ComPtr<ID3D12Resource>& ResourceIn)
	: ResourceObject(D3D12_RESOURCE_STATE_COMMON)
{
	ExitIfFailed(COM_PTR_AS_FAILED, ResourceIn.As(&Resource));
	D3D12_RESOURCE_DESC ResourceDesc = Resource->GetDesc();
	MipLevel = ResourceDesc.MipLevels;
	Format = ResourceDesc.Format;
	SetDimension(ResourceDesc.DepthOrArraySize, ResourceDesc.SampleDesc.Count > 1);
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
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
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
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}
	else
	{
		SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		UAVDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}
}
