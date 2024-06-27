#include "ResourceObject.h"

ResourceObject::ResourceObject(D3D12_RESOURCE_STATES InitialStateIn)
    : InitialState(InitialStateIn), CurrentState(InitialStateIn)
{

}

ResourceObject::~ResourceObject()
{
}

void ResourceObject::TransitionState(ID3D12GraphicsCommandList* CommandList, D3D12_RESOURCE_STATES NewState) 
{
    if (CurrentState != NewState) 
    {
        D3D12_RESOURCE_BARRIER Barrier = {};
        Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        Barrier.Transition.pResource = Resource.Get();
        Barrier.Transition.StateBefore = CurrentState;
        Barrier.Transition.StateAfter = NewState;
        Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        CommandList->ResourceBarrier(1, &Barrier);

        CurrentState = NewState;
    }
}
