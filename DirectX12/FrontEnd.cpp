#include "FrontEnd.h"

#include "DefineUtility.h"
#include "DefineErrorCode.h"
#include "Texture2DObject.h"
#include "RTVObject.h"

#include "TaskAnalyzerDialog.h"
#include "ViewportDialog.h" 

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"

#include <d3d12.h>
#include "d3dx12.h"

using namespace std;

FrontEnd::FrontEnd(ID3D12Device* Device)
{
    DeviceCached = Device;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

#pragma region Font SRV
    D3D12_DESCRIPTOR_HEAP_DESC FontSRVDesc;
    AutoZeroMemory(FontSRVDesc);

    FontSRVDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    FontSRVDesc.NumDescriptors = 1;
    FontSRVDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ExitIfFailed(CREATE_HEAP_DESCRIPTOR_FAILED, Device->CreateDescriptorHeap(&FontSRVDesc, IID_PPV_ARGS(FontSRVHeapDescriptor.GetAddressOf())));
#pragma endregion

}

FrontEnd::~FrontEnd()
{
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

void FrontEnd::Init(HWND WindowHandle)
{
    ImGui_ImplWin32_Init(WindowHandle);
    ImGui_ImplDX12_Init(DeviceCached, 1,
        DXGI_FORMAT_R8G8B8A8_UNORM, FontSRVHeapDescriptor.Get(),
        FontSRVHeapDescriptor->GetCPUDescriptorHandleForHeapStart(),
        FontSRVHeapDescriptor->GetGPUDescriptorHandleForHeapStart()
    );

#ifdef _DEBUG
    Dialogs.push_back(make_unique<TaskAnalyzerDialog>());
#endif // DEBUG

    Dialogs.push_back(make_unique<ViewportDialog>(FontSRVHeapDescriptor->GetGPUDescriptorHandleForHeapStart()));

}

void FrontEnd::Render(
    UINT FrameIndex,
    Texture2DObject* RenderTargetBaseTexture,
    RTVObject* RenderTargetObject,
    ID3D12GraphicsCommandList* CommandList
)
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto& Dialog : Dialogs)
    {
        Dialog->DoModal(DeviceCached, CommandList);
    }


    ImGui::Render();

    RenderTargetBaseTexture->TransitionState(CommandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(
        RenderTargetObject->HeapDescriptor->GetCPUDescriptorHandleForHeapStart(), 
        FrameIndex, RenderTargetObject->RTVDescriptorSize
    );
    const FLOAT ClearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    CommandList->ClearRenderTargetView(RTVHandle, ClearColor, 0, nullptr);
    CommandList->OMSetRenderTargets(1, &RTVHandle, FALSE, nullptr);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList);

    RenderTargetBaseTexture->TransitionState(CommandList, D3D12_RESOURCE_STATE_PRESENT);
}


LRESULT FrontEnd::FrontEndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}
