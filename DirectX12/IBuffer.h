#pragma once

#include "GraphicsPipeline.h"
#include "DefineUtility.h"
#include "d3dx12.h"

template<typename T>
class IBuffer
{
public:
	IBuffer();
	virtual ~IBuffer();

public:
	virtual size_t GetBufferSize() = 0;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() = 0;

public:
	virtual void Upload() = 0;
	virtual void Download() = 0;
};

template<typename T>
IBuffer<T>::IBuffer()
{
}

template<typename T>
IBuffer<T>::~IBuffer()
{
}
