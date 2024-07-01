#include "Viewable.h"
#include "DefineUtility.h"
#include "GlobalVariable.h"

using namespace std;
using namespace DirectX;

Viewable::Viewable(ID3D12Device* DeviceIn, const UINT& WidthIn, const UINT& HeightIn)
	: Object(DeviceIn), ViewBuffer(DeviceIn)
{
	Resize(WidthIn, HeightIn);
}

Viewable::~Viewable()
{
}

void Viewable::Resize(const UINT& WidthIn, const UINT& HeightIn)
{
	Width = WidthIn;
	Height = HeightIn;

	AutoZeroMemory(Viewport);
	Viewport.Width = static_cast<FLOAT>(WidthIn);
	Viewport.Height = static_cast<FLOAT>(HeightIn);
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	
	AutoZeroMemory(ScissorRect);
	ScissorRect.left = 0L;
	ScissorRect.right = static_cast<LONG>(WidthIn);
	ScissorRect.top = 0L;
	ScissorRect.bottom = static_cast<LONG>(HeightIn);
}

XMMATRIX Viewable::GetPerspectiveViewMatrix()
{
	const XMVECTOR RotationQuat = GetRotationQuat();
	XMVECTOR CurrentForward = XMVector3Rotate(Graphics::DefaultForward, RotationQuat);
	XMVECTOR CurrentUp = XMVector3Rotate(Graphics::DefaultUp, RotationQuat);

	return XMMatrixLookToLH(Position.Position, CurrentForward, CurrentUp) *
		XMMatrixPerspectiveFovLH(XMConvertToRadians(FovAngle), static_cast<float>(Width) / Height, NearZ, FarZ);
}

void Viewable::UpdateView()
{
	ViewBuffer.CPUData.ViewMatrix = GetPerspectiveViewMatrix();
	ViewBuffer.CPUData.InvViewMatrix = XMMatrixInverse(nullptr, ViewBuffer.CPUData.ViewMatrix);
	ViewBuffer.CPUData.ViewMatrix = XMMatrixTranspose(ViewBuffer.CPUData.ViewMatrix);
	ViewBuffer.Upload();
}
