#pragma once
#include "IBuffer.h"

template<typename T>
class ConstantBuffer : public IBuffer<T>
{
public:
	ConstantBuffer(const T& CPUDataIn);
	virtual ~ConstantBuffer();

public:
	alignas(256) T CPUData;

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> GPUData;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CBVHeapDescriptor;

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() { return GPUData->GetGPUVirtualAddress(); }

protected:
	CD3DX12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(CPUData));

public:
	virtual void Upload(const T& UploadData) override;
	virtual void Download() override;
};

template<typename T>
inline ConstantBuffer<T>::ConstantBuffer(const T& CPUDataIn)
	: CPUData(CPUDataIn)
{
	GraphicsPipeline::GPipeline->Device->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&GPUData)
	);

	Microsoft::WRL::ComPtr<ID3D12Resource> StagingBufferResource;
	CD3DX12_HEAP_PROPERTIES StagingHeapProperties(D3D12_HEAP_TYPE_UPLOAD);

	GraphicsPipeline::GPipeline->Device->CreateCommittedResource(
		&StagingHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&StagingBufferResource)
	);

	UINT8* pStagingDataBegin;
	CD3DX12_RANGE ReadRange(0, 0);
	StagingBufferResource->Map(0, &ReadRange, reinterpret_cast<void**>(&pStagingDataBegin));
	memcpy(pStagingDataBegin, &CPUData, sizeof(CPUData));
	StagingBufferResource->Unmap(0, nullptr);

	GraphicsPipeline::GPipeline->CommandList->CopyBufferRegion(GPUData.Get(), 0, StagingBufferResource.Get(), 0, sizeof(CPUData));
	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GPUData.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	GraphicsPipeline::GPipeline->CommandList->ResourceBarrier(1, &Barrier);

	GraphicsPipeline::GPipeline->CommandList->Close();
	ID3D12CommandList* ppCommandLists[] = { GraphicsPipeline::GPipeline->CommandList.Get() };

	GraphicsPipeline::GPipeline->CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	GraphicsPipeline::GPipeline->WaitForPreviousFrame();

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = GPUData->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = sizeof(CPUData);

	GraphicsPipeline::GPipeline->Device->CreateConstantBufferView(
		&CBVDesc, CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);
}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
}

template<typename T>
inline void ConstantBuffer<T>::Upload(const T& UploadData)
{
}

template<typename T>
inline void ConstantBuffer<T>::Download()
{
}
