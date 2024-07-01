#pragma once

#include <windows.h>
#include <wrl/client.h>

class Texture2DObject;
class RTVObject;

struct ID3D12GraphicsCommandList;

class IWorld
{
public:
	virtual void DrawToBackBuffer(
		UINT FrameIndex,
		Texture2DObject* RenderTargetBaseTexture,
		RTVObject* RenderTargetObject,
		ID3D12GraphicsCommandList* CommandList
	) = 0;
};

