#pragma once

#include "Viewable.h"
#include <memory>

class Texture2DObject;
class SRVObject;
class RTVObject;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

class Camera : public Viewable
{
public: 
	Camera(ID3D12Device* Device, const UINT& WidthIn, const UINT& HeightIn);
	virtual ~Camera();

public:
	std::unique_ptr<Texture2DObject> Texture2D;
	std::unique_ptr<SRVObject> SRV;
	std::unique_ptr<RTVObject> RTV;

public:
	void SetRenderTarget(ID3D12GraphicsCommandList* CommandList);
};

