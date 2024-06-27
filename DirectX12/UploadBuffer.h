#pragma once
#include "IBuffer.h"
#include "ResourceObject.h"

template<typename T>
class UploadBuffer : public IBuffer<T>
{
public:
	UploadBuffer(ID3D12Device* Device);
	virtual ~UploadBuffer();

public:
	alignas(256) T CPUData;

protected:
	ResourceObject GPUData;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CBVHeapDescriptor;

public:
	virtual size_t GetBufferSize() override;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() override;

protected:
	CD3DX12_HEAP_PROPERTIES HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetBufferSize());

public:
	virtual void Upload() override;
	virtual void Download() override;
};

template<typename T>
inline UploadBuffer<T>::UploadBuffer(ID3D12Device* Device)
	: IBuffer<T>(), CPUData(), GPUData(D3D12_RESOURCE_STATE_GENERIC_READ)
{
	Device->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		GPUData.GetCurrentState(),
		nullptr,
		IID_PPV_ARGS(GPUData.Resource.GetAddressOf())
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = GPUData.Resource->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = GetBufferSize();

	Device->CreateConstantBufferView(
		&CBVDesc, CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);
}

template<typename T>
inline UploadBuffer<T>::~UploadBuffer()
{
}

template<typename T>
inline size_t UploadBuffer<T>::GetBufferSize()
{
	return sizeof(CPUData);
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer<T>::GetBufferAddress() 
{
	return GPUData.Resource->GetGPUVirtualAddress(); 
}

template<typename T>
inline void UploadBuffer<T>::Upload()
{
	UINT8* pConstantBufferView = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);
	GPUData.Resource->Map(0, &ReadRange, reinterpret_cast<void**>(&pConstantBufferView));
	memcpy(pConstantBufferView, &CPUData, GetBufferSize());
	GPUData.Resource->Unmap(0, nullptr);
}

template<typename T>
inline void UploadBuffer<T>::Download()
{
}
