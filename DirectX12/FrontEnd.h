#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <vector>
#include <memory>

class Texture2DObject;
class RTVObject;

struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12DescriptorHeap;

class IDialog;

class FrontEnd
{
public:
	FrontEnd(ID3D12Device* Device);
	~FrontEnd();

protected:
	ID3D12Device* DeviceCached;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> FontSRVHeapDescriptor;

protected:
	std::vector<std::unique_ptr<IDialog>> Dialogs;

public:
	void Init(HWND WindowHandle);

	void Render(
		UINT FrameIndex,
		Texture2DObject* RenderTargetBaseTexture,
		RTVObject* RenderTargetObject,
		ID3D12GraphicsCommandList* CommandList
	);

	LRESULT FrontEndProc(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);
};

