#pragma once
#include <windows.h>
#include <DirectXMath.h>

namespace Graphics
{
	extern HWND GMainWindow;
	extern DirectX::XMVECTOR DefaultForward;
	extern DirectX::XMVECTOR DefaultUp;
	extern DirectX::XMVECTOR DefaultRight;

	extern UINT GWidth;
	extern UINT GHeight;
}