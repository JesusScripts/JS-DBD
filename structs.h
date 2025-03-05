#include <Windows.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>

float ToMeters(float x) {
    return x / 58.62f;
}

struct vec2 {
    float x, y;
};

struct Vector3 {
    float x, y, z;

    Vector3 operator-(const Vector3& ape) const {
        return { x - ape.x, y - ape.y, z - ape.z };
    }

    Vector3 operator+(const Vector3& ape) const {
        return { x + ape.x, y + ape.y, z + ape.z };
    }

    Vector3 operator*(float ape) const {
        return { x * ape, y * ape, z * ape };
    }

    Vector3 operator/(float ape) const {
        return { x / ape, y / ape, z / ape };
    }

    Vector3& operator/=(float ape) {
        x /= ape;
        y /= ape;
        z /= ape;
        return *this;
    }

    Vector3& operator+=(const Vector3& ape) {
        x += ape.x;
        y += ape.y;
        z += ape.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& ape) {
        x -= ape.x;
        y -= ape.y;
        z -= ape.z;
        return *this;
    }

    float Length() const {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    float Length2D() const {
        return std::sqrt((x * x) + (y * y));
    }

    float DistTo(const Vector3& ape) const {
        return (*this - ape).Length();
    }

    float Dist2D(const Vector3& ape) const {
        return (*this - ape).Length2D();
    }

    float Dot(const Vector3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
};

struct FVector {
    double x, y, z;

    FVector operator-(const FVector& ape) const {
        return { x - ape.x, y - ape.y, z - ape.z };
    }

    FVector operator+(const FVector& ape) const {
        return { x + ape.x, y + ape.y, z + ape.z };
    }

    FVector operator*(double ape) const {
        return { x * ape, y * ape, z * ape };
    }

    FVector operator/(double ape) const {
        return { x / ape, y / ape, z / ape };
    }

    FVector& operator/=(double ape) {
        x /= ape;
        y /= ape;
        z /= ape;
        return *this;
    }

    FVector& operator+=(const FVector& ape) {
        x += ape.x;
        y += ape.y;
        z += ape.z;
        return *this;
    }

    FVector& operator-=(const FVector& ape) {
        x -= ape.x;
        y -= ape.y;
        z -= ape.z;
        return *this;
    }

    double Length() const {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    double Length2D() const {
        return std::sqrt((x * x) + (y * y));
    }

    double DistTo(const FVector& ape) const {
        return (*this - ape).Length();
    }

    double Dist2D(const FVector& ape) const {
        return (*this - ape).Length2D();
    }

    double Dot(const FVector& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    bool IsZero() const {
        return x == 0 && y == 0 && z == 0;
    }
};

struct FQuat {
    float x, y, z, w;
};

struct FTransform {
    FQuat Rotation;
    FVector Translation;
    FVector Scale3D;

    // Optional helper method to read position directly
    FVector GetLocation() const {
        return Translation;
    }
};

typedef struct _EntityList {
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
} EntityList;

std::vector<EntityList> entityList;

struct FMinimalViewInfo {
    FVector Location;
    FVector Rotation;
    float FOV;
};

struct FCameraCacheEntry {
    float Timestamp;
    char pad_4[0xc];
    FMinimalViewInfo POV;
};

class Entity {
public:
    int nameId;
    int id;
    FVector origin;
    float distTo;
    std::string name;
    uint8_t state;
};

struct cameraMultipliers {
    float yawMultiplierX, yawMultiplierY, pitchMultiplier;
};
