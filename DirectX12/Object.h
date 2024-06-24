#pragma once
#include <DirectXMath.h>
#include "HeaderHelper.h"

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


class Object
{
public:
	Object();
	~Object();

protected:
	SPosition	Position;
	MakeGetter(Position);

protected:
	SScale		Scale;
	MakeGetter(Scale);

protected:
	SAngle		Angle;
	MakeGetter(Angle);

public:
	inline void SetPosition(const float& xIn, const float& yIn, float& zIn) { Position.x = xIn; Position.y = yIn; Position.z = zIn; Position.w = 1.f; };
	inline void SetScale(const float& xIn, const float& yIn, float& zIn) { Scale.x = xIn; Scale.y = yIn; Scale.z = zIn; Scale.w = 1.f; };
	inline void SetAngle(const float& PitchIn, const float& RollIn, float& YawIn) { Angle.Pitch = PitchIn; Angle.Roll = RollIn; Angle.Yaw = YawIn; }

public:
	DirectX::XMVECTOR GetRotationQuat();
	DirectX::XMMATRIX GetTransformation();
};

