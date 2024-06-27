#pragma once

struct ID3D12Device;
struct ID3D12GraphicsCommandList;

class IDialog
{
public:
	virtual void DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) = 0;
};

