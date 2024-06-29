#pragma once
#include "IDialog.h"

class GameWorld;

class ViewportDialog : public IDialog
{
public: 
	ViewportDialog(GameWorld* GameWorldLinkedIn);
	virtual ~ViewportDialog();

protected:
	GameWorld* GameWorldLinked = nullptr;

public:
	virtual void DoModal(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);


};

