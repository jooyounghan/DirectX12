#pragma once
#include <DirectXMath.h>
#include "HeaderHelper.h"
#include "UploadBuffer.h"

typedef struct SPosition
{
	union
	{
		DirectX::XMVECTOR Position;
		DirectX::XMVECTOR Scale;
		struct
		{
			float x, y, z, w;
		};
	};
} SScale;

struct SAngle
{
	float Pitch, Roll, Yaw;
};

struct TransformationMatrix
{
	DirectX::XMMATRIX TransfomationMat;
	DirectX::XMMATRIX InvTransfomationMat;
};

struct ID3D12Device;

class Object
{
public:
	Object(ID3D12Device* DeviceIn);
	~Object();

protected:
	SPosition	Position = SPosition();
	MakeGetter(Position);

protected:
	SScale		Scale = SScale();
	MakeGetter(Scale);

protected:
	SAngle		Angle = SAngle();
	MakeGetter(Angle);

public:
	inline void SetPosition(const float& xIn, const float& yIn, float& zIn) { Position.x = xIn; Position.y = yIn; Position.z = zIn; Position.w = 1.f; };
	inline void SetScale(const float& xIn, const float& yIn, float& zIn) { Scale.x = xIn; Scale.y = yIn; Scale.z = zIn; Scale.w = 1.f; };
	inline void SetAngle(const float& PitchIn, const float& RollIn, float& YawIn) { Angle.Pitch = PitchIn; Angle.Roll = RollIn; Angle.Yaw = YawIn; }

public:
	UploadBuffer<TransformationMatrix> TransformationBuffer;

public:
	DirectX::XMVECTOR GetRotationQuat();
	DirectX::XMMATRIX GetTransformation();

public:
	void UpdateObject();
};

