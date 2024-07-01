#include "PSOManager.h"
#include "DefineErrorCode.h"
#include "DefineUtility.h"

#include <d3dcompiler.h>
#include "d3dx12.h"
//#include <d3d12sdklayers.h>
//#include <dxgi1_4.h>

using namespace Microsoft::WRL;

PSOManager::PSOManager(ID3D12Device* DeviceIn)
{
#pragma region Root Signature
    CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
    RootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ExitIfFailed(CREATE_ROOT_SIGNATURE_FAILED, D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ExitIfFailed(CREATE_ROOT_SIGNATURE_FAILED, DeviceIn->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(),
        IID_PPV_ARGS(RootSignature.GetAddressOf()))
    );
#pragma endregion

    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(L"./Shaders/BaseVS.hlsl", nullptr, nullptr, "main", "vs_5_0", CompileFlags, 0, &vertexShader, nullptr));
        ExitIfFailed(CREATE_SHADER_FAILED, D3DCompileFromFile(L"./Shaders/BasePS.hlsl", nullptr, nullptr, "main", "ps_5_0", CompileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = RootSignature.Get();
        psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ExitIfFailed(CREATE_PSO_FAILED, DeviceIn->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PipelineState)));
    }
}

PSOManager::~PSOManager()
{
}
