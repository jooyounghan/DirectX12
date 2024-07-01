#include "Map.h"
#include "RenderObject.h"

#include "Camera.h"
#include "Texture2DObject.h"
#include "RTVObject.h"

using namespace std;

Map::Map(ID3D12Device* DeviceIn, ID3D12GraphicsCommandList* CommandListIn)
	: PSOManagerInstance(DeviceIn)
{
	// TODO : 테스트라 삭제 필요
	std::vector<Vertex> Vertices
	{
		Vertex{Point{-0.0f, 0.25f, 0}, Color{1.f, 0.f, 0.f, 1.f}},
		Vertex{Point{0.25f, -0.25f, 0}, Color{0.f, 1.f, 0.f, 1.f}},
		Vertex{Point{0.25f, -0.25f, 0}, Color{0.f, 0.f, 1.f, 1.f}},
	};

	RenderObjects.emplace_back(make_unique<RenderObject>(Vertices, DeviceIn, CommandListIn));
}

Map::~Map()
{
}

void Map::UpdateMap(const float& DeltaTimeIn)
{
	for (const auto& ro : RenderObjects)
	{
		ro->UpdateObject();
	}
}

void Map::CaptureMapByCamera(Camera* Camera, ID3D12GraphicsCommandList* CommandListIn)
{
	CommandListIn->SetPipelineState(PSOManagerInstance.PipelineState.Get());
	CommandListIn->SetGraphicsRootSignature(PSOManagerInstance.RootSignature.Get());
	Camera->Texture2D->TransitionState(CommandListIn, D3D12_RESOURCE_STATE_RENDER_TARGET);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(
		Camera->RTV->HeapDescriptor->GetCPUDescriptorHandleForHeapStart(),
		0, Camera->RTV->RTVDescriptorSize
	);

	const FLOAT ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	CommandListIn->ClearRenderTargetView(RTVHandle, ClearColor, 0, nullptr);
	CommandListIn->OMSetRenderTargets(1, &RTVHandle, FALSE, nullptr);
	CommandListIn->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& ro : RenderObjects)
	{
		CommandListIn->IASetVertexBuffers(0, 1, &(ro->VertexBuffer.GetBufferView()));
		CommandListIn->DrawInstanced(3, 1, 0, 0);
	}
}
