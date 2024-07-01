#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <d3d12.h>

enum class PSOType
{
	Default
};

class PSOManager
{
public:
	PSOManager(ID3D12Device* DeviceIn);
	~PSOManager();

public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;

};

