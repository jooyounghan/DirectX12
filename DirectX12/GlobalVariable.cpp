#include "GlobalVariable.h"

using namespace DirectX;

HWND Graphics::GMainWindow = NULL;
XMVECTOR Graphics::DefaultForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
XMVECTOR Graphics::DefaultUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
XMVECTOR Graphics::DefaultRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);