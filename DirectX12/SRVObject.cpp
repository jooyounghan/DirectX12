#include "SRVObject.h"
#include "GraphicsPipeline.h"
#include "DefineUtility.h"
#include "DefineErrorCode.h"

SRVObject::SRVObject(
    ID3D12Device* Device,
    UINT ResourceCount,
    ID3D12Resource** Resources
)
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
    AutoZeroMemory(HeapDesc);
    HeapDesc.NumDescriptors = ResourceCount;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ExitIfFailed(CREATE_HEAP_DESCRIPTOR_FAILED, Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(HeapDescriptor.GetAddressOf())));

    SRVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE SRVHandle(HeapDescriptor->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < ResourceCount; n++)
    {
        Device->CreateShaderResourceView(Resources[n], nullptr, SRVHandle);
        SRVHandle.Offset(1, SRVDescriptorSize);
    }
}

SRVObject::~SRVObject()
{
}
