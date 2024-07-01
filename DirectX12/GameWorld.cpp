#include "GameWorld.h"
#include "GlobalVariable.h"

#include "Texture2DObject.h"
#include "SRVObject.h"
#include "RTVObject.h"
#include "Camera.h"
#include "Map.h"

using namespace std;

GameWorld::GameWorld(ID3D12Device* DeviceIn, ID3D12GraphicsCommandList* CommandListIn)
{

	// TODO : 에디터 모드와 게임 모드에서 카메라 위치 변경
#ifdef _DEBUG
	EditorViewCamera = make_unique<Camera>(DeviceIn, Graphics::GWidth, Graphics::GHeight);
	CurrentCamera = EditorViewCamera.get();
	MapInstances.emplace(0, std::move(make_unique<Map>(DeviceIn, CommandListIn)));
	CurrentMap = MapInstances[0].get();
#else
	EditorViewCamera = make_unique<Camera>(DeviceIn, Graphics::GWidth, Graphics::GHeight);
	CurrentCamera = EditorViewCamera.get();
#endif // _DEBUG

}

GameWorld::~GameWorld()
{
}

void GameWorld::LoadGameWorld()
{
	// Load Current State Of GameWorld From Binary Files
}

void GameWorld::UpdateGameWorld(
	const float& DeltaTimeIn,
	ID3D12GraphicsCommandList* CommandListIn
)
{
	if (CurrentCamera) CurrentCamera->UpdateView();
	if (CurrentMap)
	{
		CurrentMap->UpdateMap(DeltaTimeIn);
		CurrentMap->CaptureMapByCamera(CurrentCamera, CommandListIn);
	}
}

void GameWorld::DrawToBackBuffer(
	UINT FrameIndex, 
	Texture2DObject* RenderTargetBaseTexture, 
	RTVObject* RenderTargetObject, 
	ID3D12GraphicsCommandList* CommandList
)
{
	if (CurrentCamera != nullptr)
	{
		CommandList->CopyResource(RenderTargetBaseTexture->GetResource(), GetCurrentViewResource());
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE GameWorld::GetCurrentViewHandle()
{
	return CurrentCamera ? CurrentCamera->SRV->HeapDescriptor->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE();
}

ID3D12Resource* GameWorld::GetCurrentViewResource()
{
	return CurrentCamera ? CurrentCamera->Texture2D->GetResource() : nullptr;
}
