#pragma once
#include "IBuffer.h"

template<typename T>
class UploadBuffer : public IBuffer<T>
{
public:
	UploadBuffer(D3D12_RESOURCE_STATES ResourceState);
	virtual ~UploadBuffer();

public:
	alignas(256) T CPUData;

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> GPUData;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CBVHeapDescriptor;

public:
	inline D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() { return GPUData->GetGPUVirtualAddress(); }

protected:
	CD3DX12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(CPUData));

public:
	virtual void Upload(const T& UploadData) override;
	virtual void Download() override;
};

template<typename T>
inline UploadBuffer<T>::UploadBuffer(D3D12_RESOURCE_STATES ResourceState)
{
	GraphicsPipeline::GPipeline->Device->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		ResourceState,
		nullptr,
		IID_PPV_ARGS(&GPUData)
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = GPUData->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = sizeof(CPUData);

	GraphicsPipeline::GPipeline->Device->CreateConstantBufferView(
		&CBVDesc, CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);
}

template<typename T>
inline UploadBuffer<T>::~UploadBuffer()
{
}

template<typename T>
inline void UploadBuffer<T>::Upload(const T& UploadData)
{
	UINT8* pCBVDataBegin = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);

	GPUData->Map(0, &ReadRange, reinterpret_cast<void**>(&pCBVDataBegin));
	memcpy(pCBVDataBegin, CPUData.Get(), sizeof(CPUData));
	GPUData->Unmap(0, nullptr);
}

template<typename T>
inline void UploadBuffer<T>::Download()
{
}
