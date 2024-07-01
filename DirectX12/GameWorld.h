#pragma once
#include "IWorld.h"

#include <d3d12.h>
#include "d3dx12.h"
#include <memory>
#include <unordered_map>

class Map;
class Camera;

class GameWorld : public IWorld
{
public:
	GameWorld(ID3D12Device* DeviceIn, ID3D12GraphicsCommandList* CommandListIn);
	~GameWorld();

public:
	void LoadGameWorld();
	void UpdateGameWorld(
		const float& DeltaTimeIn,
		ID3D12GraphicsCommandList* CommandListIn
	);

protected:
	/*
	온라인 게임 변경의 경우 서버 인스턴스에서
	맵의 정보를 요청 받아서 사용	
	*/
	std::unordered_map<UINT, std::unique_ptr<Map>> MapInstances;
	Map* CurrentMap = nullptr;

public:
	virtual void DrawToBackBuffer(
		UINT FrameIndex,
		Texture2DObject* RenderTargetBaseTexture,
		RTVObject* RenderTargetObject,
		ID3D12GraphicsCommandList* CommandList
	) override;

	// TODO : 에디터 모드와 게임 모드에서 카메라 설정 변경
#ifdef _DEBUG
protected:
	std::unique_ptr<Camera> EditorViewCamera;
#else
	std::unique_ptr<Camera> EditorViewCamera;
#endif // _DEBUG

protected:
	Camera* CurrentCamera = nullptr;

public:
	D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentViewHandle();
	ID3D12Resource*				GetCurrentViewResource();
};

