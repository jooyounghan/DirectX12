#pragma once

#include "Viewable.h"
#include <memory>

class Texture2DObject;
class SRVObject;
class RTVObject;

class Camera : public Viewable
{
public: 
	Camera(const float& WidthIn, const float& HeightIn);
	virtual ~Camera();

public:
	std::unique_ptr<Texture2DObject> Texture2D;
	std::unique_ptr<SRVObject> SRV;
	std::unique_ptr<RTVObject> RTV;

public:
	void SetRenderTarget();
};

