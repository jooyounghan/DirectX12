#pragma once

#include "Viewable.h"

class Camera : public Viewable
{
public: 
	Camera(const float& WidthIn, const float& HeightIn);
	virtual ~Camera();

};

