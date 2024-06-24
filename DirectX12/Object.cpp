#include "Object.h"

using namespace DirectX;

Object::Object()
{

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
