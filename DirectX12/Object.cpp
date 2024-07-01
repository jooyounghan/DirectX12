#include "Object.h"
#include "DefineUtility.h"

using namespace DirectX;

Object::Object(ID3D12Device* DeviceIn)
	: TransformationBuffer(DeviceIn)
{
	AutoZeroMemory(Position);
	AutoZeroMemory(Scale);
	AutoZeroMemory(Angle);
}

Object::~Object()
{
}

DirectX::XMVECTOR Object::GetRotationQuat()
{
	return XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(Angle.Pitch),
		XMConvertToRadians(Angle.Yaw),
		XMConvertToRadians(Angle.Roll)
	);
}

DirectX::XMMATRIX Object::GetTransformation()
{
	return XMMatrixAffineTransformation(
		Scale.Scale, 
		XMQuaternionIdentity(), 
		GetRotationQuat(),
		Position.Position
	);
}

void Object::UpdateObject()
{
	// TODO : 테스트라 삭제
	Angle.Roll += 0.1f;

	TransformationBuffer.CPUData.TransfomationMat = GetTransformation();
	TransformationBuffer.CPUData.InvTransfomationMat = XMMatrixInverse(nullptr, TransformationBuffer.CPUData.TransfomationMat);
	TransformationBuffer.CPUData.TransfomationMat = XMMatrixTranspose(TransformationBuffer.CPUData.TransfomationMat);
	TransformationBuffer.Upload();
}
