#include "RTVObject.h"
#include "GraphicsPipeline.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

RTVObject::RTVObject(
    ID3D12Device* Device,
    UINT ResourceCount,
	ID3D12Resource** Resources
)
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
    AutoZeroMemory(HeapDesc);
    HeapDesc.NumDescriptors = ResourceCount;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ExitIfFailed(CREATE_HEAP_DESCRIPTOR_FAILED, Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(HeapDescriptor.GetAddressOf())));

    RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(HeapDescriptor->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < ResourceCount; n++)
    {
        Device->CreateRenderTargetView(Resources[n], nullptr, RTVHandle);
        RTVHandle.Offset(1, RTVDescriptorSize);
    }
}

RTVObject::~RTVObject()
{
}
