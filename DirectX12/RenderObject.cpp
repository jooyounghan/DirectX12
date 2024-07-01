#include "RenderObject.h"

RenderObject::RenderObject(
	const std::vector<Vertex>& Vertices,
	ID3D12Device* DeviceIn,
	ID3D12GraphicsCommandList* CommandList
)
	: Object(DeviceIn), VertexBuffer(Vertices, DeviceIn, CommandList)
{
}

RenderObject::RenderObject(
	std::vector<Vertex>&& Vertices,
	ID3D12Device* DeviceIn,
	ID3D12GraphicsCommandList* CommandList
)
	: Object(DeviceIn), VertexBuffer(Vertices, DeviceIn, CommandList)
{
}

RenderObject::~RenderObject()
{
}
