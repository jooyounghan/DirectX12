#pragma once

#include "GraphicsPipeline.h"
#include "DefineErrorCode.h"
#include "DefineUtility.h"

template<typename T>
class IBuffer
{
public:
	IBuffer(ID3D12Device* Device);
	virtual ~IBuffer();

protected:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CBVHeapDescriptor;

public:
	virtual size_t GetBufferSize() = 0;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() = 0;

public:
	virtual void Upload() = 0;
	virtual void Download() = 0;
};

template<typename T>
IBuffer<T>::IBuffer(ID3D12Device* Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC CBVHeapDesc = {};
	CBVHeapDesc.NumDescriptors = 1;
	CBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	CBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ExitIfFailed(CREATE_HEAP_DESCRIPTOR_FAILED, Device->CreateDescriptorHeap(&CBVHeapDesc, IID_PPV_ARGS(CBVHeapDescriptor.GetAddressOf())));
}

template<typename T>
IBuffer<T>::~IBuffer()
{
}
