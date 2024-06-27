#include "TaskAnalyzerDialog.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"

TaskAnalyzerDialog::TaskAnalyzerDialog()
{
}

TaskAnalyzerDialog::~TaskAnalyzerDialog()
{
}

void TaskAnalyzerDialog::DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Task Analyzer");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}
