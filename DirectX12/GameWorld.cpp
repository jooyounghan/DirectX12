#include "GameWorld.h"
#include "GlobalVariable.h"

#include "Texture2DObject.h"
#include "SRVObject.h"
#include "Camera.h"
#include "Map.h"

using namespace std;

GameWorld::GameWorld(ID3D12Device* DeviceIn)
{
#ifdef _DEBUG
	//EditorViewCamera = make_unique<Camera>(DeviceIn, Graphics::GWidth, Graphics::GHeight);
#endif // _DEBUG

}

GameWorld::~GameWorld()
{
}

void GameWorld::LoadGameWorld()
{
	// Load Current State Of GameWorld From Binary Files
}

void GameWorld::UpdateGameWorld(const float& DeltaTimeIn)
{
	if (CurrentMap != nullptr)
	{
		// Update Map		
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
