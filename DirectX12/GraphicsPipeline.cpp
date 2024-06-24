#include "GraphicsPipeline.h"
#include "GlobalVariable.h"
#include "DefineErrorCode.h"
#include "DefineUtility.h"

#include <filesystem>
#include <shlobj.h>

using namespace std;
using namespace Microsoft::WRL;

GraphicsPipeline* GraphicsPipeline::GPipeline = nullptr;

GraphicsPipeline::GraphicsPipeline()
{
    GPipeline = this;
}

GraphicsPipeline::~GraphicsPipeline()
{
}

std::wstring GraphicsPipeline::GetLatestWinPixGpuCapturerPath()
{
    LPWSTR programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);
    filesystem::path pixInstallationPath = programFilesPath;
    pixInstallationPath /= "Microsoft PIX";

    std::wstring newestVersionFound;

    for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
    {
        if (directory_entry.is_directory())
        {
            if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
            {
                newestVersionFound = directory_entry.path().filename().c_str();
            }
        }
    }

    if (newestVersionFound.empty())
    {
        // TODO: Error, no PIX installation found
    }

    return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}

void GraphicsPipeline::LoadPipeline(const UINT& WidthIn, const UINT& HeightIn)
{
    SetAspectRatio(static_cast<float>(WidthIn) / HeightIn);

    if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
    {
        LoadLibrary(GetLatestWinPixGpuCapturerPath().c_str());
    }

#pragma region Debug Layer
#ifdef _DEBUG
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(DebugController.GetAddressOf()))))
    {
        DebugController->EnableDebugLayer();
    }
#endif // DEBUG
#pragma endregion
       
#pragma region Device
    ComPtr<IDXGIFactory4> Factory;
    ExitIfFailed(CREATE_DXGI_FACTORY_FAILED, CreateDXGIFactory1(IID_PPV_ARGS(Factory.GetAddressOf())));

    ComPtr<IDXGIAdapter> WarpAdapter;
    ExitIfFailed(GET_WARP_ADAPTER_FAILED, Factory->EnumWarpAdapter(IID_PPV_ARGS(WarpAdapter.GetAddressOf())));

    ExitIfFailed(CREATE_DEVICE_FAILED, D3D12CreateDevice(
        WarpAdapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(Device.GetAddressOf())
    ));
#pragma endregion

#pragma region Command Queue
    D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
    AutoZeroMemory(CommandQueueDesc);
    CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    CommandQueueDesc.Type = CommandListType;

    ExitIfFailed(CREATE_COMMAND_QUEUE_FAILED, Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(CommandQueue.GetAddressOf())));
#pragma endregion

#pragma region Command Allocator
    ExitIfFailed(CREATE_COMMAND_ALLOCATOR_FAILED, Device->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
#pragma endregion

#pragma region Swapchain
    DXGI_SWAP_CHAIN_DESC SwapChainDesc;
    AutoZeroMemory(SwapChainDesc);
    SwapChainDesc.BufferCount = BackBufferCount;
    SwapChainDesc.BufferDesc.Width = WidthIn;
    SwapChainDesc.BufferDesc.Height = HeightIn;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    SwapChainDesc.OutputWindow = Graphics::GMainWindow;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain> TempSwapChain;
    ExitIfFailed(CREATE_SWAP_CHAIN_FAILED, Factory->CreateSwapChain(
        CommandQueue.Get(),
        &SwapChainDesc,
        &TempSwapChain
    ));
    ExitIfFailed(CREATE_SWAP_CHAIN_FAILED, TempSwapChain.As(&SwapChain));
#pragma endregion

#pragma region Heap Descriptor
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
    AutoZeroMemory(HeapDesc);
    HeapDesc.NumDescriptors = BackBufferCount;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ExitIfFailed(CREATE_HEAP_DESCRIPTOR_FAILED, Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(RTVHeapDescriptor.GetAddressOf())));
#pragma endregion

#pragma region Render Target View
    RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHandle(RTVHeapDescriptor->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < BackBufferCount; n++)
    {
        ExitIfFailed(GET_BUFFER_FROM_SWAPCHAIN_FAILED, SwapChain->GetBuffer(n, IID_PPV_ARGS(RenderTargets[n].GetAddressOf())));
        Device->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RTVHandle);
        RTVHandle.Offset(1, RTVDescriptorSize);
    }
#pragma endregion
}

void GraphicsPipeline::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. More advanced samples 
    // illustrate how to use fences for efficient resource usage.

    // Signal and increment the fence value.
    const UINT64 fence = FenceValue;
    ExitIfFailed(WAIT_FOR_PREV_FRAME_FAILED, CommandQueue->Signal(Fence.Get(), fence));
    FenceValue++;

    // Wait until the previous frame is finished.
    if (Fence->GetCompletedValue() < fence)
    {
        ExitIfFailed(WAIT_FOR_PREV_FRAME_FAILED, Fence->SetEventOnCompletion(fence, FenceEvent));
        WaitForSingleObject(FenceEvent, INFINITE);
    }

    FrameIndex = SwapChain->GetCurrentBackBufferIndex();
}

void GraphicsPipeline::LoadAsset()
{
#pragma region Root Signature
    CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
    RootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ExitIfFailed(CREATE_ROOT_SIGNATURE_FAILED, D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ExitIfFailed(CREATE_ROOT_SIGNATURE_FAILED, Device->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(),
        IID_PPV_ARGS(RootSignature.GetAddressOf()))
    );
#pragma endregion

#pragma region Pipeline
    ComPtr<ID3DBlob> VertexShaderBlob;
    ComPtr<ID3DBlob> PixelShaderBlob;
    ComPtr<ID3DBlob> VertexErrorBlob;
    ComPtr<ID3DBlob> PixelErrorBlob;

#if defined(_DEBUG)
    UINT CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT CompileFlags = NULL;
#endif

    ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(
        L"./Shaders/BaseVS.hlsl", nullptr, nullptr, "main", "vs_5_0",
        CompileFlags, 0, VertexShaderBlob.GetAddressOf(), VertexErrorBlob.GetAddressOf())
    );
    ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(
        L"./Shaders/BasePS.hlsl", nullptr, nullptr, "main", "ps_5_0",
        CompileFlags, 0, PixelShaderBlob.GetAddressOf(), PixelErrorBlob.GetAddressOf())
    );

    D3D12_INPUT_ELEMENT_DESC InputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
    AutoZeroMemory(PSODesc);
    PSODesc.InputLayout = { InputElementDescs, _countof(InputElementDescs) };
    PSODesc.pRootSignature = RootSignature.Get();
    PSODesc.VS = { reinterpret_cast<UINT8*>(VertexShaderBlob->GetBufferPointer()), VertexShaderBlob->GetBufferSize() };
    PSODesc.PS = { reinterpret_cast<UINT8*>(PixelShaderBlob->GetBufferPointer()), PixelShaderBlob->GetBufferSize() };
    PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSODesc.DepthStencilState.DepthEnable = FALSE;
    PSODesc.DepthStencilState.StencilEnable = FALSE;
    PSODesc.SampleMask = UINT_MAX;
    PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSODesc.NumRenderTargets = 1;
    PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    PSODesc.SampleDesc.Count = 1;
    ExitIfFailed(CREATE_PSO_FAILED, Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(PipelineState.GetAddressOf())));
#pragma endregion

#pragma region Command List
    ExitIfFailed(CREATE_COMMAND_LIST_FAILED, Device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
        PipelineState.Get(), IID_PPV_ARGS(CommandList.GetAddressOf()))
    );
    ExitIfFailed(CLOSE_COMMAND_LIST_FAILED, CommandList->Close());
#pragma endregion

#pragma region Vertex Buffer
    // Define the geometry for a triangle.
    Vertex TriangleVertices[] =
    {
        { { 0.0f, 0.25f * AspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * AspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * AspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    const UINT VertexBufferSize = sizeof(TriangleVertices);

    CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto Desc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
    ExitIfFailed(CREATE_COMMITTED_RESOURCE_FAILED, Device->CreateCommittedResource(
        &HeapProps,
        D3D12_HEAP_FLAG_NONE,
        &Desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(VertexBuffer.GetAddressOf()))
    );

    // Copy the triangle data to the vertex buffer.
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE ReadRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ExitIfFailed(MAP_RESOURCE_FAILED, VertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, TriangleVertices, sizeof(TriangleVertices));
    VertexBuffer->Unmap(0, nullptr);

    // Initialize the vertex buffer view.
    VertexBufferView = std::make_unique<D3D12_VERTEX_BUFFER_VIEW>();
    VertexBufferView->BufferLocation = VertexBuffer->GetGPUVirtualAddress();
    VertexBufferView->StrideInBytes = sizeof(Vertex);
    VertexBufferView->SizeInBytes = VertexBufferSize;
#pragma endregion

#pragma region Synchronization Object
    ExitIfFailed(CREATE_FENCE_FAILED, Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf())));
    FenceValue = 1;

    // Create an event handle to use for frame synchronization.
    FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (FenceEvent == nullptr)
    {
        ExitIfFailed(CREATE_FENCE_FAILED, HRESULT_FROM_WIN32(GetLastError()));
    }
    WaitForPreviousFrame();
#pragma endregion
}

void GraphicsPipeline::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
    CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ExitIfFailed(SWAPCHAIIN_PRESENT_FAILED, SwapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void GraphicsPipeline::PopulateCommandList()
{
    ExitIfFailed(RESET_COMMAND_FAILED, CommandAllocator->Reset());
    ExitIfFailed(RESET_COMMAND_FAILED, CommandList->Reset(CommandAllocator.Get(), PipelineState.Get()));

    CommandList->SetGraphicsRootSignature(RootSignature.Get());

    CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        RenderTargets[FrameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT, 
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    CommandList->ResourceBarrier(1, &Barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(RTVHeapDescriptor->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RTVDescriptorSize);
    CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CommandList->IASetVertexBuffers(0, 1, VertexBufferView.get());
    CommandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        RenderTargets[FrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, 
        D3D12_RESOURCE_STATE_PRESENT
    );
    CommandList->ResourceBarrier(1, &Barrier);

    ExitIfFailed(RESET_COMMAND_FAILED, CommandList->Close());
}
