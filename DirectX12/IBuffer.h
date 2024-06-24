#pragma once

#include "GraphicsPipeline.h"
#include "DefineUtility.h"

#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include "directx/d3dx12.h"

template<typename T>
class IBuffer
{
public:
	virtual void Upload() = 0;
	virtual void Download() = 0;
};