#include "driver.hpp"
#include "structs.h"
#include "overlay.h"
#include <vector>


uintptr_t uWorld = 0;

uintptr_t gameInstance = 0;

uintptr_t persistentLevel = 0;

uintptr_t localPlayerPtr = 0;

uintptr_t localPlayer = 0;

uintptr_t playerController = 0;

FVector lastCameraPos = {};

uintptr_t PlayerStateLocalPlayer = 0;

FCameraCacheEntry tempCamCache;

uintptr_t Localpawn = 0;

uintptr_t APlayerState = 0;

uintptr_t cameraManager = 0;

FCameraCacheEntry cameraCache = {};

uintptr_t actorsArray = 0;

std::string nameS;

int actorsCount = 0;

std::vector<Entity> players2Draw = {};
std::vector<Entity> misc2Draw = {};

bool freecamera = false;

HWND hwnd = NULL;
HWND hwnd_active = NULL;
HWND OverlayWindow = NULL;
auto CrosshairColor = D2D1::ColorF(0, 100, 255, 255);
auto Color = D2D1::ColorF(255, 0, 0, 255);
#define M_PI 3.14159265358979323846264338327950288419716939937510

bool esp = true;
bool espLine = false;
bool infoEsp = false;
bool name = false;
bool box = false;
bool drawMisc = false;
bool localIsSlasher = false;

int counter;

bool corneredBox = false;

bool showmenu = true;
bool rendering = true;
int frame = 0;

FVector localOrigin = {};

float sHeight;
float sWidth;

float ScreenCenterX;
float ScreenCenterY;

float FOV = 40;
float distanceMax = 250;

uint64_t process_base = 0;
uint32_t process_id = 0;



