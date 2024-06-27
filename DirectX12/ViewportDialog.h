#pragma once
#include "IDialog.h"

#include "d3d12.h"

class ViewportDialog : public IDialog
{
public: 
	ViewportDialog(D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle);
	virtual ~ViewportDialog();

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE Handle;

public:
	virtual void DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);


};

