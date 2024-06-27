#include "ViewportDialog.h"


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"

ViewportDialog::ViewportDialog(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle)
    : Handle(GPUHandle)
{
}

ViewportDialog::~ViewportDialog()
{
}

void ViewportDialog::DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
    ImGui::Begin("View Port");
    
    ImGui::Image((void*)Handle.ptr, ImGui::GetContentRegionAvail());

    ImGui::End();
}
