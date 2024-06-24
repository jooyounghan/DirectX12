#include "Camera.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

#include "Texture2DObject.h"
#include "SRVObject.h"
#include "RTVObject.h"

Camera::Camera(const float& WidthIn, const float& HeightIn)
	: Viewable(WidthIn, HeightIn)
{
	Texture2D = std::make_unique<Texture2DObject>();
}

Camera::~Camera()
{
}

void Camera::SetRenderTarget()
{
	GraphicsPipeline::GPipeline->CommandList->RSSetViewports(1, &Viewport);
	GraphicsPipeline::GPipeline->CommandList->RSSetScissorRects(1, &ScissorRect);
}
