#pragma once

#include "PSOManager.h"

#include <vector>
#include <memory>

class RenderObject;

struct ID3D12Device;
struct ID3D12GraphicsCommandList;

class Camera;

class Map
{
public:
	Map(ID3D12Device* DeviceIn, ID3D12GraphicsCommandList* CommandListIn);
	~Map();

protected:
	/* 
	�ʿ��� �����ϴ� ��ü
	�ܺ� ��� �ؽ���, ���� �ν��Ͻ�, �������� �÷��̾� ���� ����	
	*/

protected:
	PSOManager PSOManagerInstance;

protected:
	std::vector<std::unique_ptr<RenderObject>> RenderObjects;

public:
	void UpdateMap(const float& DeltaTimeIn);
	void CaptureMapByCamera(Camera* Camera, ID3D12GraphicsCommandList* CommandListIn);
};

