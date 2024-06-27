#include "RenderObject.h"

RenderObject::RenderObject(
	const std::vector<Vertex>& Vertices,
	ID3D12Device* Device,
	ID3D12GraphicsCommandList* CommandList
)
	: VertexBuffer(Vertices, Device, CommandList)
{
}

RenderObject::RenderObject(
	std::vector<Vertex>&& Vertices,
	ID3D12Device* Device,
	ID3D12GraphicsCommandList* CommandList
)
	: VertexBuffer(Vertices, Device, CommandList)
{
}

RenderObject::~RenderObject()
{
}
