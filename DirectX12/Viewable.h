#pragma once
#include "Object.h"
#include <d3d12.h>

class Viewable : public Object
{
public:
	Viewable(const float& WidthIn, const float& HeightIn);
	virtual ~Viewable();

protected:
	float Width = 0;
	float Height = 0;

public:
	D3D12_VIEWPORT	Viewport;
	D3D12_RECT		ScissorRect;

protected:
	float FovAngle	= 90.f;
	MakeAddressGetter(FovAngle);
	float NearZ		= 0.1f;
	MakeAddressGetter(NearZ);
	float FarZ		= 1000.f;
	MakeAddressGetter(FarZ);

public:
	virtual void Resize(const float& WidthIn, const float& HeightIn);
	DirectX::XMMATRIX GetPerspectiveViewMatrix();
};
