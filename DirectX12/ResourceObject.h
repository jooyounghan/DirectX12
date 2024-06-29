#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include "HeaderHelper.h"

class ResourceObject 
{
public:
    ResourceObject(D3D12_RESOURCE_STATES InitialStateIn);
    virtual ~ResourceObject();

public:
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource;

protected:
    D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES CurrentState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    MakeGetter(CurrentState);

public:
    void TransitionState(ID3D12GraphicsCommandList* CommandList, D3D12_RESOURCE_STATES NewState);
    inline void SetDefaultState(ID3D12GraphicsCommandList* CommandList) { return TransitionState(CommandList, InitialState); }
};