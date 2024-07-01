#include "GraphicsPipeline.h"
#include "GlobalVariable.h"
#include "DefineErrorCode.h"
#include "DefineUtility.h"
#include "Texture2DObject.h"
#include "SRVObject.h"
#include "RTVObject.h"

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
    ExitIfFailed(COM_PTR_AS_FAILED, TempSwapChain.As(&SwapChain));
#pragma endregion

#pragma region Back Buffer 
    vector<ID3D12Resource*> Resources;
    for (UINT n = 0; n < BackBufferCount; n++)
    {
        ComPtr<ID3D12Resource> TempBackBuffer;
        ExitIfFailed(GET_BUFFER_FROM_SWAPCHAIN_FAILED, SwapChain->GetBuffer(n, IID_PPV_ARGS(TempBackBuffer.GetAddressOf())));
        BackBufferTextures[n] = make_unique<Texture2DObject>(TempBackBuffer);
        Resources.push_back(BackBufferTextures[n]->Resource.Get());
    }
    BackBufferRTVs = make_unique<RTVObject>(Device.Get(), BackBufferCount, Resources.data());
#pragma endregion

#pragma region Command List
    ExitIfFailed(CREATE_COMMAND_LIST_FAILED, Device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
        nullptr, IID_PPV_ARGS(CommandList.GetAddressOf()))
    );
    //ExitIfFailed(RESET_COMMAND_FAILED, CommandList->Close());
#pragma endregion

#pragma region Synchronization Object
    ExitIfFailed(CREATE_FENCE_FAILED, Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.GetAddressOf())));
    FenceValue = 1;
    FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (FenceEvent == nullptr)
    {
        ExitIfFailed(CREATE_FENCE_FAILED, HRESULT_FROM_WIN32(GetLastError()));
    }
    WaitForPreviousFrame();
#pragma endregion
}

void GraphicsPipeline::PrepareRender()
{

}

void GraphicsPipeline::ExecuteRender()
{
    ExitIfFailed(RESET_COMMAND_FAILED, CommandList->Close());

    ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
    CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    ExitIfFailed(SWAPCHAIIN_PRESENT_FAILED, SwapChain->Present(1, 0));

    WaitForPreviousFrame();


    ExitIfFailed(RESET_COMMAND_FAILED, CommandAllocator->Reset());
    ExitIfFailed(RESET_COMMAND_FAILED, CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void GraphicsPipeline::WaitForPreviousFrame()
{
    const UINT64 fence = FenceValue;
    ExitIfFailed(WAIT_FOR_PREV_FRAME_FAILED, CommandQueue->Signal(Fence.Get(), fence));
    FenceValue++;

    if (Fence->GetCompletedValue() < fence)
    {
        ExitIfFailed(WAIT_FOR_PREV_FRAME_FAILED, Fence->SetEventOnCompletion(fence, FenceEvent));
        WaitForSingleObject(FenceEvent, INFINITE);
    }

    FrameIndex = SwapChain->GetCurrentBackBufferIndex();
}

//void GraphicsPipeline::LoadAsset()
//{
//#pragma region Pipeline
//    ComPtr<ID3DBlob> VertexShaderBlob;
//    ComPtr<ID3DBlob> PixelShaderBlob;
//    ComPtr<ID3DBlob> VertexErrorBlob;
//    ComPtr<ID3DBlob> PixelErrorBlob;
//
//#if defined(_DEBUG)
//    UINT CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//    UINT CompileFlags = NULL;
//#endif
//
//    ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(
//        L"./Shaders/BaseVS.hlsl", nullptr, nullptr, "main", "vs_5_0",
//        CompileFlags, 0, VertexShaderBlob.GetAddressOf(), VertexErrorBlob.GetAddressOf())
//    );
//    ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(
//        L"./Shaders/BasePS.hlsl", nullptr, nullptr, "main", "ps_5_0",
//        CompileFlags, 0, PixelShaderBlob.GetAddressOf(), PixelErrorBlob.GetAddressOf())
//    );
//
//    D3D12_INPUT_ELEMENT_DESC InputElementDescs[] =
//    {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
//    };
//
//    // Describe and create the graphics pipeline state object (PSO).
//    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
//    AutoZeroMemory(PSODesc);
//    PSODesc.InputLayout = { InputElementDescs, _countof(InputElementDescs) };
//    PSODesc.pRootSignature = RootSignature.Get();
//    PSODesc.VS = { reinterpret_cast<UINT8*>(VertexShaderBlob->GetBufferPointer()), VertexShaderBlob->GetBufferSize() };
//    PSODesc.PS = { reinterpret_cast<UINT8*>(PixelShaderBlob->GetBufferPointer()), PixelShaderBlob->GetBufferSize() };
//    PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//    PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//    PSODesc.DepthStencilState.DepthEnable = FALSE;
//    PSODesc.DepthStencilState.StencilEnable = FALSE;
//    PSODesc.SampleMask = UINT_MAX;
//    PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    PSODesc.NumRenderTargets = 1;
//    PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
//    PSODesc.SampleDesc.Count = 1;
//    ExitIfFailed(CREATE_PSO_FAILED, Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(PipelineState.GetAddressOf())));
//#pragma endregion
//
//
//#pragma region Vertex Buffer
//    // Define the geometry for a triangle.
//    Vertex TriangleVertices[] =
//    {
//        { { 0.0f, 0.25f * AspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
//        { { 0.25f, -0.25f * AspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
//        { { -0.25f, -0.25f * AspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
//    };
//
//    const UINT VertexBufferSize = sizeof(TriangleVertices);
//
//    CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_UPLOAD);
//    auto Desc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
//    ExitIfFailed(CREATE_COMMITTED_RESOURCE_FAILED, Device->CreateCommittedResource(
//        &HeapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &Desc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(VertexBuffer.GetAddressOf()))
//    );
//
//    // Copy the triangle data to the vertex buffer.
//    UINT8* pVertexDataBegin;
//    CD3DX12_RANGE ReadRange(0, 0);        // We do not intend to read from this resource on the CPU.
//    ExitIfFailed(MAP_RESOURCE_FAILED, VertexBuffer->Map(0, &ReadRange, reinterpret_cast<void**>(&pVertexDataBegin)));
//    memcpy(pVertexDataBegin, TriangleVertices, sizeof(TriangleVertices));
//    VertexBuffer->Unmap(0, nullptr);
//
//    // Initialize the vertex buffer view.
//    VertexBufferView = std::make_unique<D3D12_VERTEX_BUFFER_VIEW>();
//    VertexBufferView->BufferLocation = VertexBuffer->GetGPUVirtualAddress();
//    VertexBufferView->StrideInBytes = sizeof(Vertex);
//    VertexBufferView->SizeInBytes = VertexBufferSize;
//#pragma endregion
//
//}