#include "ViewportDialog.h"

#include "GameWorld.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"

ViewportDialog::ViewportDialog(GameWorld* GameWorldLinkedIn)
    : GameWorldLinked(GameWorldLinkedIn)
{
}

ViewportDialog::~ViewportDialog()
{
}

void ViewportDialog::DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
    ImGui::Begin("View Port");
    if (UINT64 HandlerPtr = GameWorldLinked->GetCurrentViewHandle().ptr)
    {
        ImGui::Image((void*)HandlerPtr, ImGui::GetContentRegionAvail());
    }
    ImGui::End();
}
