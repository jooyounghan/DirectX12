#pragma once
#include "Object.h"
#include "UploadBuffer.h"
 
struct ViewBufferData
{
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX InvViewMatrix;
};

class Viewable : public Object
{
public:
	Viewable(ID3D12Device* DeviceIn, const UINT& WidthIn, const UINT& HeightIn);
	virtual ~Viewable();

protected:
	UINT Width = 0;
	UINT Height = 0;

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
	virtual void Resize(const UINT& WidthIn, const UINT& HeightIn);
	DirectX::XMMATRIX GetPerspectiveViewMatrix();

public:
	UploadBuffer<ViewBufferData> ViewBuffer;

public:
	void UpdateView();
};

