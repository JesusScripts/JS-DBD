
#include <vector>
#include <chrono>
#include <cmath>
#include <iostream>


static D3DMATRIX CreateMatrix(FVector rot) {
    const float DEG_TO_RAD = static_cast<float>(M_PI) / 180.f;
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

static int ScreenCenterX = 1080; // Example value, adjust as needed
static int ScreenCenterY = 1920; // Example value, adjust as needed



bool WorldToScreen(const FMinimalViewInfo& camera, const FVector& WorldLocation, POINT& Screenlocation) {
    // Get screen dimensions instead of using hardcoded values
    int ScreenWidth = settings::width;
    int ScreenHeight = settings::height;
    int ScreenCenterX = ScreenWidth / 2;
    int ScreenCenterY = ScreenHeight / 2;

    const D3DMATRIX tempMatrix = CreateMatrix(camera.Rotation);

    FVector vAxisX{ tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2] };
    FVector vAxisY{ tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2] };
    FVector vAxisZ{ tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2] };

    FVector vDelta = WorldLocation - camera.Location;
    FVector vTransformed = FVector{ vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX) };

    if (vTransformed.z < 1.f)
        return false; // Object is behind camera

    const float FOV_DEG_TO_RAD = static_cast<float>(M_PI) / 360.f;
    float FovAngle = camera.FOV;

    // Calculate screen position with proper aspect ratio handling
    Screenlocation.x = static_cast<LONG>(ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * FOV_DEG_TO_RAD)) / vTransformed.z);
    Screenlocation.y = static_cast<LONG>(ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * FOV_DEG_TO_RAD)) / vTransformed.z);

    // Check if the point is within screen bounds
    return (Screenlocation.x >= 0 && Screenlocation.x <= ScreenWidth && Screenlocation.y >= 0 && Screenlocation.y <= ScreenHeight);
}