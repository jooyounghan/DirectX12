#pragma once
#include "Object.h"
#include "ConstantArrayBuffer.h"

struct Point
{
	float x, y, z;
};

struct Color
{
	float r, g, b, a;
};

struct Vertex
{
	Point PointValue;
	Color ColorValue;
};

class RenderObject : public Object
{
public:
	RenderObject(
		const std::vector<Vertex>& Vertices, 
		ID3D12Device* Device,
		ID3D12GraphicsCommandList* CommandList
	);
	RenderObject(
		std::vector<Vertex>&& Vertices, 
		ID3D12Device* Device,
		ID3D12GraphicsCommandList* CommandList
	);
	virtual ~RenderObject();

public:
	ConstantArrayBuffer<Vertex> VertexBuffer;
};

