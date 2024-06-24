#include "RTVObject.h"
#include "GraphicsPipeline.h"
#include "DefineUtility.h"

RTVObject::RTVObject(
	ID3D12Resource* pResource, 
	DXGI_FORMAT FormatIn, 
	D3D12_RTV_DIMENSION ViewDimensionIn
)
{
    D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
    AutoZeroMemory(RTVDesc);

    RTVDesc.Format = FormatIn;
    RTVDesc.ViewDimension = ViewDimensionIn;
    RTVDesc.Texture2D.MipSlice = 0;
    RTVDesc.Texture2D.PlaneSlice = 0;

    GraphicsPipeline::GPipeline->Device->CreateRenderTargetView(
        pResource, &RTVDesc, RTVHandle
    );
}

RTVObject::~RTVObject()
{
}
