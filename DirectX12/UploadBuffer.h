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

public:
	virtual size_t GetBufferSize() override;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() override;

protected:
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetBufferSize());

public:
	virtual void Upload() override;
	virtual void Download() override;
};

template<typename T>
inline UploadBuffer<T>::UploadBuffer(ID3D12Device* Device)
	: IBuffer<T>(Device, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_TYPE_UPLOAD), CPUData()
{
	Device->CreateCommittedResource(
		&this->GPUHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&this->ResourceDesc,
		this->GPUData.GetCurrentState(),
		nullptr,
		IID_PPV_ARGS(this->GPUData.Resource.GetAddressOf())
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = this->GPUData.Resource->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = static_cast<UINT>(GetBufferSize());

	Device->CreateConstantBufferView(
		&CBVDesc, this->CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);

	this->GPUData.ResourceView.BufferLocation = this->GPUData.Resource->GetGPUVirtualAddress();
	this->GPUData.ResourceView.StrideInBytes = sizeof(T);
	this->GPUData.ResourceView.SizeInBytes = sizeof(T);
}

template<typename T>
inline UploadBuffer<T>::~UploadBuffer()
{
}

template<typename T>
inline size_t UploadBuffer<T>::GetBufferSize()
{
	return  (sizeof(CPUData) + 255) & ~255;
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer<T>::GetBufferAddress() 
{
	return this->GPUData.Resource->GetGPUVirtualAddress();
}

template<typename T>
inline void UploadBuffer<T>::Upload()
{
	UINT8* pConstantBufferView = nullptr;
	CD3DX12_RANGE ReadRange(0, 0);
	this->GPUData.Resource->Map(0, &ReadRange, reinterpret_cast<void**>(&pConstantBufferView));
	memcpy(pConstantBufferView, &CPUData, GetBufferSize());
	this->GPUData.Resource->Unmap(0, nullptr);
}

template<typename T>
inline void UploadBuffer<T>::Download()
{
}
