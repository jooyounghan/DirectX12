#pragma once
#include "IBuffer.h"
#include "ResourceObject.h"
#include <vector>

template<typename T>
class ConstantArrayBuffer : public IBuffer<T>
{
public:
	ConstantArrayBuffer(
		const std::vector<T>& CPUDataIn, 
		ID3D12Device* Device,
		ID3D12GraphicsCommandList* CommandList
	);
	ConstantArrayBuffer(
		std::vector<T>&& CPUDataIn, 
		ID3D12Device* Device,
		ID3D12GraphicsCommandList* CommandList
	);
	virtual ~ConstantArrayBuffer();

public:
	std::vector<T> CPUData;

protected:
	ResourceObject GPUData;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CBVHeapDescriptor;
	CD3DX12_HEAP_PROPERTIES GPUHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

protected:
	ResourceObject StagingResource;
	CD3DX12_HEAP_PROPERTIES StagingHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

public:
	virtual size_t GetBufferSize() override;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() override;;

protected:
	CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetBufferSize());

protected:
	void InitBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);

public:
	virtual void Upload() override;
	virtual void Download() override;
};

template<typename T>
inline ConstantArrayBuffer<T>::ConstantArrayBuffer(
	const std::vector<T>& CPUDataIn, 
	ID3D12Device* Device,
	ID3D12GraphicsCommandList* CommandList
)
	: IBuffer<T>(), CPUData(CPUDataIn), GPUData(D3D12_RESOURCE_STATE_COMMON), StagingResource(D3D12_RESOURCE_STATE_GENERIC_READ)
{
	InitBuffer(Device, CommandList);
}

template<typename T>
inline ConstantArrayBuffer<T>::ConstantArrayBuffer(
	std::vector<T>&& CPUDataIn, 
	ID3D12Device* Device,
	ID3D12GraphicsCommandList* CommandList
)
	: IBuffer<T>(), CPUData(CPUDataIn), GPUData(D3D12_RESOURCE_STATE_COMMON), StagingResource(D3D12_RESOURCE_STATE_GENERIC_READ)
{
	InitBuffer(Device, CommandList);
}

template<typename T>
inline ConstantArrayBuffer<T>::~ConstantArrayBuffer()
{
}

template<typename T>
inline size_t ConstantArrayBuffer<T>::GetBufferSize() 
{ 
	return CPUData.size() * sizeof(T); 
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS ConstantArrayBuffer<T>::GetBufferAddress() 
{ 
	return GPUData.Resource->GetGPUVirtualAddress();
}

template<typename T>
inline void ConstantArrayBuffer<T>::InitBuffer(
	ID3D12Device* Device, 
	ID3D12GraphicsCommandList* CommandList
)
{
	Device->CreateCommittedResource(
		&GPUHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		GPUData.GetCurrentState(),
		nullptr,
		IID_PPV_ARGS(GPUData.Resource.GetAddressOf())
	);

	Device->CreateCommittedResource(
		&StagingHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		StagingResource.GetCurrentState(),
		nullptr,
		IID_PPV_ARGS(StagingResource.Resource.GetAddressOf())
	);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	AutoZeroMemory(CBVDesc);
	CBVDesc.BufferLocation = GPUData.Resource->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = GetBufferSize();

	GraphicsPipeline::GPipeline->Device->CreateConstantBufferView(
		&CBVDesc, CBVHeapDescriptor->GetCPUDescriptorHandleForHeapStart()
	);

	UINT8* pStagingBufferView;
	CD3DX12_RANGE ReadRange(0, 0);
	StagingResource.Resource->Map(0, &ReadRange, reinterpret_cast<void**>(&pStagingBufferView));
	memcpy(pStagingBufferView, CPUData.data(), GetBufferSize());
	StagingResource.Resource->Unmap(0, nullptr);

	StagingResource.TransitionState(CommandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	GPUData.TransitionState(CommandList, D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->CopyBufferRegion(GPUData.Resource.Get(), 0, StagingResource.Resource.Get(), 0, GetBufferSize());
	GPUData.SetDefaultState(CommandList);
	StagingResource.SetDefaultState(CommandList);
}

template<typename T>
inline void ConstantArrayBuffer<T>::Upload()
{
}

template<typename T>
inline void ConstantArrayBuffer<T>::Download()
{
}
