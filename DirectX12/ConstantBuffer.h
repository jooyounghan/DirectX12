#pragma once
#include "IBuffer.h"
#include "ResourceObject.h"

template<typename T>
class ConstantBuffer : public IBuffer<T>
{
public:
	ConstantBuffer(
		const T& CPUDataIn, 
		ID3D12Device* Device, 
		ID3D12GraphicsCommandList* CommandList
	);
	virtual ~ConstantBuffer();

public:
	alignas(256) T CPUData;

protected:
	ResourceObject GPUData;
	CD3DX12_HEAP_PROPERTIES GPUHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

protected:
	ResourceObject StagingResource;
	CD3DX12_HEAP_PROPERTIES StagingHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

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
inline ConstantBuffer<T>::ConstantBuffer(
	const T& CPUDataIn,
	ID3D12Device* Device,
	ID3D12GraphicsCommandList* CommandList
)
	: IBuffer<T>(Device), CPUData(CPUDataIn), GPUData(D3D12_RESOURCE_STATE_COMMON), StagingResource(D3D12_RESOURCE_STATE_GENERIC_READ)
{
	Device->CreateCommittedResource(
		&GPUHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		GPUData.CurrentState,
		nullptr,
		IID_PPV_ARGS(GPUData.Resource.GetAddressOf())
	);

	Device->CreateCommittedResource(
		&StagingHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		StagingResource.CurrentState,
		nullptr,
		IID_PPV_ARGS(StagingResource.Resource.GetAddressOf())
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = GPUData->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = GetBufferSize();

	Device->CreateConstantBufferView(
		&CBVDesc, this->CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);

	UINT8* pStagingBufferView;
	CD3DX12_RANGE ReadRange(0, 0);
	StagingResource->Map(0, &ReadRange, reinterpret_cast<void**>(&pStagingBufferView));
	memcpy(pStagingBufferView, &CPUData, GetBufferSize());
	StagingResource->Unmap(0, nullptr);

	StagingResource.TransitionState(CommandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	GPUData.TransitionState(CommandList, D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->CopyBufferRegion(GPUData.Get(), 0, StagingResource.Get(), 0, GetBufferSize());
	GPUData.SetDefaultState();
	StagingResource.SetDefaultState();
}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
}

template<typename T>
inline size_t ConstantBuffer<T>::GetBufferSize()
{ 
	return  (sizeof(CPUData) + 255) & ~255;
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer<T>::GetBufferAddress() 
{ 
	return GPUData.Resource->GetGPUVirtualAddress();
}

template<typename T>
inline void ConstantBuffer<T>::Upload()
{
}

template<typename T>
inline void ConstantBuffer<T>::Download()
{
}
