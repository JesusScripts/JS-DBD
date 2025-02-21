#include "offset.h"
#include <vector>

typedef struct _D3DMATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
} D3DMATRIX;

static D3DMATRIX CreateMatrix(FVector rot) {
	const float DEG_TO_RAD = static_cast<double>(3.14159265358979323846) / 180.f;
	const float radPitch = rot.x * DEG_TO_RAD;
	const float radYaw = rot.y * DEG_TO_RAD;
	const float radRoll = rot.z * DEG_TO_RAD;

	const float SP = sinf(radPitch);
	const float CP = cosf(radPitch);
	const float SY = sinf(radYaw);
	const float CY = cosf(radYaw);
	const float SR = sinf(radRoll);
	const float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = 0.f;
	matrix.m[3][1] = 0.f;
	matrix.m[3][2] = 0.f;
	matrix.m[3][3] = 1.f;

	return matrix;
}



static POINT WorldToScreen(FMinimalViewInfo camera, FVector WorldLocation)
{
	POINT Screenlocation{0, 0};
	const D3DMATRIX tempMatrix = CreateMatrix(camera.Rotation);


	FVector vAxisX{ tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2] };
	FVector vAxisY{ tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2] };
	FVector vAxisZ{ tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2] };

	FVector vDelta = WorldLocation - camera.Location;
	FVector vTransformed = FVector{ vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX) };

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	const float FOV_DEG_TO_RAD = static_cast<float>(3.14159265358979323846) / 360.f;


	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(camera.FOV * FOV_DEG_TO_RAD)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(camera.FOV * FOV_DEG_TO_RAD)) / vTransformed.z;

	return Screenlocation;
}


static std::string GetNameById(uint32_t actor_id)
{
	char pNameBuffer[256];
	int TableLocation = (unsigned int)(actor_id >> 0x10);
	uint16_t RowLocation = (unsigned __int16)actor_id;
	uint64_t GNameTable = process_base + offsets::GNames;

	uint64_t TableLocationAddress = read<uint64_t>(GNameTable + 0x10 + TableLocation * 0x8) + (unsigned __int32)(4 * RowLocation);

	uint64_t sLength = (unsigned __int64)(read<uint16_t>(TableLocationAddress + 4)) >> 1;

	if (sLength < 128)
	{
		read_array<char>(TableLocationAddress + 6, pNameBuffer, sLength);
		return std::string(pNameBuffer, sLength);
	}
	return std::string("NULL");
}
