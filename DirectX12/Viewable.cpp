#include "Viewable.h"
#include "DefineUtility.h"
#include "GlobalVariable.h"

using namespace std;
using namespace DirectX;

Viewable::Viewable(const float& WidthIn, const float& HeightIn)
{
	Resize(WidthIn, HeightIn);
}

Viewable::~Viewable()
{
}

void Viewable::Resize(const float& WidthIn, const float& HeightIn)
{
	Width = WidthIn;
	Height = HeightIn;

	AutoZeroMemory(Viewport);
	Viewport.Width = WidthIn;
	Viewport.Height = HeightIn;
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
		XMMatrixPerspectiveFovLH(XMConvertToRadians(FovAngle), Width / Height, NearZ, FarZ);
}
