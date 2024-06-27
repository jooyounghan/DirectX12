#include "Camera.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

#include "Texture2DObject.h"
#include "SRVObject.h"
#include "RTVObject.h"

Camera::Camera(ID3D12Device* Device, const float& WidthIn, const float& HeightIn)
	: Viewable(Device, WidthIn, HeightIn)
{
	Texture2D = std::make_unique<Texture2DObject>(
		Width, Height, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, false,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	ID3D12Resource* Resources[1] = { Texture2D->GetResource() };

	SRV = std::make_unique<SRVObject>(
		Device,
		1, Resources
	);

	RTV = std::make_unique<RTVObject>(
		Device,
		1, Resources
	);
}

Camera::~Camera()
{
}

void Camera::SetRenderTarget(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
}
