#pragma once
#include "IDialog.h"

class TaskAnalyzerDialog : public IDialog
{
public:
	TaskAnalyzerDialog();
	virtual ~TaskAnalyzerDialog();

public:
	virtual void DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
};

