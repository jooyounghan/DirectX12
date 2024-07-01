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
	맵에서 관리하는 객체
	외부 배경 텍스쳐, 몬스터 인스턴스, 접속중인 플레이어 등을 관리	
	*/

protected:
	PSOManager PSOManagerInstance;

protected:
	std::vector<std::unique_ptr<RenderObject>> RenderObjects;

public:
	void UpdateMap(const float& DeltaTimeIn);
	void CaptureMapByCamera(Camera* Camera, ID3D12GraphicsCommandList* CommandListIn);
};

