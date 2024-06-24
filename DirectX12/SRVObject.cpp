#include "SRVObject.h"
#include "GraphicsPipeline.h"
#include "DefineUtility.h"

SRVObject::SRVObject(
    ID3D12Resource* pResource,
    DXGI_FORMAT FormatIn,
    D3D12_SRV_DIMENSION ViewDimensionIn,
    UINT MipLevelsIn
)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	AutoZeroMemory(SRVDesc);

    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.Format = FormatIn;
    SRVDesc.ViewDimension = ViewDimensionIn;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = MipLevelsIn;
    SRVDesc.Texture2D.PlaneSlice = 0;
    SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	GraphicsPipeline::GPipeline->Device->CreateShaderResourceView(
        pResource, &SRVDesc, SRVHandle
    );
}

SRVObject::~SRVObject()
{
}
