#pragma once

#include "IWorld.h"

#include <vector>
#include <memory>

class GameWorld;

struct ID3D12Device;
struct ID3D12DescriptorHeap;

class IDialog;

class EditorWorld : public IWorld
{
public:
	EditorWorld(ID3D12Device* DeviceIn, GameWorld* GameWorldIn);
	~EditorWorld();

protected:
	ID3D12Device* DeviceCached = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> FontSRVHeapDescriptor;

protected:
	GameWorld* GameWorldLinked = nullptr;

protected:
	std::vector<std::unique_ptr<IDialog>> Dialogs;

public:
	void Init(HWND WindowHandle);

public:
	virtual void DrawToBackBuffer(
		UINT FrameIndex,
		Texture2DObject* RenderTargetBaseTexture,
		RTVObject* RenderTargetObject,
		ID3D12GraphicsCommandList* CommandList
	) override;

public:
	LRESULT EditorWorldProc(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);
};

