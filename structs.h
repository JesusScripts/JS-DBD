#include <Windows.h>
#include <vector>
#include <iostream>

float ToMeters(float x)
{
	return x / 58.62f;
}


struct vec2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
	Vector3 operator-(Vector3 ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	Vector3 operator+(Vector3 ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	Vector3 operator*(float ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	Vector3 operator/(float ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	Vector3 operator/=(float ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	Vector3 operator+=(Vector3 ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	Vector3 operator-=(Vector3 ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	float Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float DistTo(Vector3 ape)
	{
		return (*this - ape).Length();
	}

	float Dist2D(Vector3 ape)
	{
		return (*this - ape).Length2D();
	}

	float Dot(Vector3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}  
};

struct FVector
{
	double x, y, z;

	FVector operator-(FVector ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	FVector operator+(FVector ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	FVector operator*(double ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	FVector operator/(double ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	FVector operator/=(double ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	FVector operator+=(FVector ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	FVector operator-=(FVector ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	double Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	double DistTo(FVector ape)
	{
		return (*this - ape).Length();
	}

	double Dist2D(FVector ape)
	{
		return (*this - ape).Length2D();
	}

	double Dot(FVector& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
};

typedef struct _EntityList
{
	uintptr_t instance;
	uintptr_t mesh;
	uintptr_t root_component;
	uintptr_t instigator;
	uintptr_t PlayerState;
	uintptr_t Pawn;
	Vector3 TopLocation;
	Vector3 bone_origin;
	std::string name;
	Vector3 origin;
	float health;
	float dist;
	int objectId;
	int team;
}EntityList;

std::vector<EntityList> entityList;

struct FMinimalViewInfo
{
	FVector Location; // 0x00(0x0c)
	FVector Rotation; // 0x0c(0x0c)
	float FOV; // 0x18(0x04)

};

;

struct FCameraCacheEntry
{
	float Timestamp; // 0x00(0x04)
	char pad_4[0xc]; // 0x04(0x0c)
	FMinimalViewInfo POV; // 0x10(0x5e0)
};

class Entity {
private:
public:
	int nameId;
	int id;
	FVector origin;
	int distTo;
};

struct cameraMultipliers
{
	float yawMultiplierX, yawMultiplierY, pitchMultiplier;
};