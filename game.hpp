#pragma warning(disable : 4996)

#include "protect/crypt.h"
#include "crypt.h"
#include <shared_mutex>
#include "imgui/imgui.h"
//#include "offsets.hpp"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <utility>
#include <windows.h>
#include <fstream>
#include <algorithm> // Add this line
#include <limits> // Add this line
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <set>
#include <algorithm> // For std::min and std::max
#include <cstddef>   // For size_t
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <map> // Add this line at the top of your file
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <regex>


// Global DirectX objects.
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { 0 };
MSG messager = { 0 };
HWND my_wnd = NULL;
HWND game_wnd = NULL;
DWORD processID;
FCameraCacheEntry cameraCache;
bool StoreObjects = true;


std::shared_mutex entityMutex;
uintptr_t GWorld = 0, GName = 0;
uintptr_t Localpawn = 0;
int localId = 0;
FVector localOrigin;
bool localIsSlasher = false;


class FName
{
public:
    static inline void* GNames = nullptr;

    uint32_t ComparisonIndex;
    uint32_t Number;

    inline uint32_t GetDisplayIndex() const
    {
        return ComparisonIndex;
    }

    inline bool operator==(const FName& Other) const
    {
        return ComparisonIndex == Other.ComparisonIndex && Number == Other.Number;
    }

    inline bool operator!=(const FName& Other) const
    {
        return ComparisonIndex != Other.ComparisonIndex || Number != Other.Number;
    }
};

std::string GetNameById(uint32_t actor_id) {
    char pNameBuffer[256];
    int TableLocation = (unsigned int)(actor_id >> 0x10);
    uint16_t RowLocation = (unsigned __int16)actor_id;
    uint64_t GNameTable = virtualaddy + O::GName;
    uint64_t TableLocationAddress = read<uint64_t>(GNameTable + 0x10 + TableLocation * 0x8) + (unsigned __int32)(4 * RowLocation);
    uint64_t sLength = (unsigned __int64)(read<uint16_t>(TableLocationAddress + 4)) >> 1;
    if (sLength < 128) {
        mem::read_physical(TableLocationAddress + 6, pNameBuffer, sLength);
        return std::string(pNameBuffer);
    }
    return std::string("NULL");
}


bool IsValidPtr(uintptr_t ptr) {
    return (ptr != 0 && ptr != 0xFFFFFFFFFFFFFFFF && ptr > 0x10000);
}

// Simplified version of Aura function
void Aura(uintptr_t obj, uint32_t addr, const float color[3]) {
    if (!obj) return;

    auto outLine = read<uintptr_t>(obj + addr);
    if (!outLine) return;

    write<float>(outLine + 0x3B4, color[0]);
    write<float>(outLine + 0x3B8, color[1]);
    write<float>(outLine + 0x3BC, color[2]);
    write<float>(outLine + 0x3C0, 1.f);

}

HRESULT directx_init()
{
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
        exit(3);
    ZeroMemory(&p_params, sizeof(p_params));
    p_params.Windowed = TRUE;
    p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p_params.hDeviceWindow = my_wnd;
    p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
    p_params.BackBufferWidth = settings::width;
    p_params.BackBufferHeight = settings::height;
    p_params.EnableAutoDepthStencil = TRUE;
    p_params.AutoDepthStencilFormat = D3DFMT_D16;
    p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
    {
        p_object->Release();
        exit(4);
    }
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(my_wnd);
    ImGui_ImplDX9_Init(p_device);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = 0;
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowBorderSize = 1;
    style->FrameBorderSize = 1;
    style->WindowTitleAlign = { 0.5f, 0.5f };
    style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
    style->Colors[ImGuiCol_Border] = ImColor(8, 8, 8, 250);
    style->Colors[ImGuiCol_TitleBg] = ImColor(38, 38, 38, 250);
    style->Colors[ImGuiCol_TitleBgActive] = ImColor(38, 38, 38, 250);
    style->Colors[ImGuiCol_WindowBg] = ImColor(38, 38, 38, 250);
    style->Colors[ImGuiCol_FrameBg] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_Button] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 38, 245);
    style->Colors[ImGuiCol_SliderGrab] = ImColor(68, 68, 68, 250);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(68, 68, 68, 250);
    style->Colors[ImGuiCol_CheckMark] = ImColor(68, 68, 68, 250);
    style->Colors[ImGuiCol_Header] = ImColor(68, 68, 68, 250);
    style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(68, 68, 68, 250);
    style->Colors[ImGuiCol_PopupBg] = ImColor(38, 38, 38, 250);
    p_object->Release();
    return S_OK;
}

void create_overlay()
{
    WNDCLASSEXA wcex = {
        sizeof(WNDCLASSEXA),
        0,
        DefWindowProcA,
        0,
        0,
        0,
        LoadIcon(0, IDI_APPLICATION),
        LoadCursor(0, IDC_ARROW),
        0,
        0,
        "SofMainFNBase",
        LoadIcon(0, IDI_APPLICATION)
    };
    ATOM rce = RegisterClassExA(&wcex);
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    my_wnd = gui::create_window_in_band(0, rce, L"SofMainFNBase", WS_POPUP,
        rect.left, rect.top, rect.right, rect.bottom,
        0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
    SetWindowLong(my_wnd, GWL_EXSTYLE,
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(my_wnd, &margin);
    ShowWindow(my_wnd, SW_SHOW);
    UpdateWindow(my_wnd);
}

void Box(int X, int Y, int W, int H, const ImColor color, int thickness)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2((float)X, (float)Y),
        ImVec2((float)(X + W), (float)Y), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2((float)(X + W), (float)Y),
        ImVec2((float)(X + W), (float)(Y + H)), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2((float)X, (float)(Y + H)),
        ImVec2((float)(X + W), (float)(Y + H)), color, thickness);
    ImGui::GetForegroundDrawList()->AddLine(ImVec2((float)X, (float)Y),
        ImVec2((float)X, (float)(Y + H)), color, thickness);
}

using namespace std::chrono;

namespace Cache {
    // Generic object cache.
    struct Object {
        uintptr_t actor;
        struct FVector location;
        Object(uintptr_t a, const struct FVector& loc) : actor(a), location(loc) {}
        uintptr_t GetObj() const { return actor; }
        struct FVector GetLocation() const { return location; }
    };

    // Containers for various game objects.
    std::vector<Object*> Players;
    std::vector<Object*> Killers;
    std::vector<Object*> Chests;
    std::vector<Object*> Pallets;
    std::vector<Object*> Gens;
    std::vector<Object*> Hooks;
    std::vector<Object*> Windows;
    std::vector<Object*> KillerItems;
    std::vector<Object*> Hatch;
    std::vector<Object*> Totem;
}

namespace AuraCache {
    // Generic object cache.
    struct Object {
        uintptr_t actor;
        struct FVector location;
        Object(uintptr_t a, const struct FVector& loc) : actor(a), location(loc) {}
        uintptr_t GetObj() const { return actor; }
        struct FVector GetLocation() const { return location; }
    };

    // Containers for various game objects.
    std::vector<Object*> Players;
    std::vector<Object*> Killers;
    std::vector<Object*> Chests;
    std::vector<Object*> Pallets;
    std::vector<Object*> Gens;
    std::vector<Object*> Hooks;
    std::vector<Object*> Windows;
    std::vector<Object*> KillerItems;
    std::vector<Object*> Hatch;
    std::vector<Object*> Totem;
}

enum class EPalletState : uint8_t {
    Up = 0,
    Falling = 1,
    Fallen = 2,
    Destroyed = 3,
    EPalletState_MAX = 4,
};

void DrawESP() {
    if (!settings::esp::enable) return;

    auto* drawList = ImGui::GetForegroundDrawList();

    // Optimized distance calculation
    auto calculateDistance = [](const FVector& point1, const FVector& point2) -> float {
        float dx = point2.x - point1.x;
        float dy = point2.y - point1.y;
        float dz = point2.z - point1.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0f;
        };

    // Text drawing helpers
    auto drawTextWithShadow = [&](const ImVec2& pos, ImColor color, const char* text) {
        if (settings::esp::text_shadow) {
            drawList->AddText(ImVec2(pos.x + 1, pos.y + 1), ImColor(0, 0, 0, 200), text);
        }
        drawList->AddText(pos, color, text);
        };

    auto drawColoredText = [&](const ImVec2& pos, const char* text, ImColor color) {
        if (!text || text[0] == '\0') return;

        int textLen = strlen(text);
        float startX = pos.x;

        for (int i = 0; i < textLen; i++) {
            char singleChar[2] = { text[i], '\0' };
            ImColor charColor = settings::esp::rainbow_text ?
                ImColor::HSV(ImGui::GetTime() * 0.5f + i * 0.05f, 0.8f, 0.8f) :
                color;

            if (settings::esp::text_shadow) {
                drawList->AddText(ImVec2(startX + 1, pos.y + 1), ImColor(0, 0, 0, 200), singleChar);
            }
            drawList->AddText(ImVec2(startX, pos.y), charColor, singleChar);

            startX += ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, singleChar).x;
        }
        };

    // Player/Killer box drawing function with per-entity type options
    auto drawBox = [&](
        const POINT& screenHead,
        int width,
        int height,
        ImColor color,
        const char* label,
        float distance,
        float health,
        bool isPlayer,
        bool showBox,
        bool showName,
        bool showDistance,
        bool showHealth
        ) {
            float x = static_cast<float>(screenHead.x);
            float y = static_cast<float>(screenHead.y);

            ImVec2 topLeft(x - width / 2, y);
            ImVec2 bottomRight(x + width / 2, y + height);

            // Only draw box elements if the box is enabled
            if (showBox) {
                if (settings::esp::filled_boxes) {
                    ImColor fillColor = color;
                    fillColor.Value.w = settings::esp::box_alpha;
                    drawList->AddRectFilled(topLeft, bottomRight, fillColor);
                }

                drawList->AddRect(topLeft, bottomRight, color, 0.0f, 0, settings::esp::outline_thickness);

                if (settings::esp::show_corner_lines) {
                    float cl = settings::esp::corner_line_length;
                    float thickness = settings::esp::outline_thickness;

                    // Top left
                    drawList->AddLine(topLeft, ImVec2(topLeft.x + cl, topLeft.y), color, thickness);
                    drawList->AddLine(topLeft, ImVec2(topLeft.x, topLeft.y + cl), color, thickness);

                    // Top right
                    ImVec2 topRight(bottomRight.x, topLeft.y);
                    drawList->AddLine(topRight, ImVec2(topRight.x - cl, topRight.y), color, thickness);
                    drawList->AddLine(topRight, ImVec2(topRight.x, topRight.y + cl), color, thickness);

                    // Bottom left
                    ImVec2 bottomLeft(topLeft.x, bottomRight.y);
                    drawList->AddLine(bottomLeft, ImVec2(bottomLeft.x + cl, bottomLeft.y), color, thickness);
                    drawList->AddLine(bottomLeft, ImVec2(bottomLeft.x, bottomLeft.y - cl), color, thickness);

                    // Bottom right
                    drawList->AddLine(bottomRight, ImVec2(bottomRight.x - cl, bottomRight.y), color, thickness);
                    drawList->AddLine(bottomRight, ImVec2(bottomRight.x, bottomRight.y - cl), color, thickness);
                }
            }

            // Name display
            if (showName && label) {
                drawTextWithShadow(ImVec2(topLeft.x, topLeft.y - 14 - 2), ImColor(255, 255, 255, 255), label);
            }

            // Distance display
            if (showDistance && distance >= 0) {
                char distStr[32];
                sprintf_s(distStr, "%.1fm", distance);
                drawTextWithShadow(ImVec2(topLeft.x, bottomRight.y + 2), ImColor(255, 255, 255, 255), distStr);
            }

            // Health bar
            if (showHealth && health >= 0) {
                float healthBarWidth = width;
                float healthBarHeight = 4.0f;
                float currentHealthWidth = healthBarWidth * health;

                drawList->AddRectFilled(
                    ImVec2(topLeft.x, topLeft.y - healthBarHeight - 4),
                    ImVec2(topLeft.x + healthBarWidth, topLeft.y - 4),
                    ImColor(35, 35, 35, 180)
                );

                ImColor healthColor;
                if (health > 0.6f) healthColor = ImColor(0, 255, 0, 255);
                else if (health > 0.3f) healthColor = ImColor(255, 255, 0, 255);
                else healthColor = ImColor(255, 0, 0, 255);

                drawList->AddRectFilled(
                    ImVec2(topLeft.x, topLeft.y - healthBarHeight - 4),
                    ImVec2(topLeft.x + currentHealthWidth, topLeft.y - 4),
                    healthColor
                );
            }
        };

    // Draw world objects (optimized version - code reuse for similar objects)
    auto drawWorldObject = [&](
        const FVector& origin,
        float distance,
        const char* label,
        ImColor color,
        int objectType // 0=circle, 1=diamond, 2=triangle, 3=X
        ) {
            if (distance > settings::esp::max_render_distance)
                return;

            POINT screenPos;
            if (!WorldToScreen(cameraCache.POV, origin, screenPos))
                return;

            char infoText[64];
            sprintf_s(infoText, "%s [%.1fm]", label, distance);

            // Draw object icon based on type
            const float size = (objectType == 3) ? 3.0f : 5.0f;

            switch (objectType) {
            case 0: // Circle
                drawList->AddCircleFilled(ImVec2(screenPos.x, screenPos.y), 3.0f, color);
                break;

            case 1: { // Diamond
                ImVec2 diamondPoints[4] = {
                    ImVec2(screenPos.x, screenPos.y - size),
                    ImVec2(screenPos.x + size, screenPos.y),
                    ImVec2(screenPos.x, screenPos.y + size),
                    ImVec2(screenPos.x - size, screenPos.y)
                };
                drawList->AddConvexPolyFilled(diamondPoints, 4, color);
                break;
            }

            case 2: { // Triangle
                ImVec2 trianglePoints[3] = {
                    ImVec2(screenPos.x, screenPos.y - size),
                    ImVec2(screenPos.x + size, screenPos.y + size),
                    ImVec2(screenPos.x - size, screenPos.y + size)
                };
                drawList->AddTriangleFilled(trianglePoints[0], trianglePoints[1], trianglePoints[2], color);
                break;
            }

            case 3: // X mark
                drawList->AddLine(
                    ImVec2(screenPos.x - size, screenPos.y - size),
                    ImVec2(screenPos.x + size, screenPos.y + size),
                    color, 1.5f
                );
                drawList->AddLine(
                    ImVec2(screenPos.x + size, screenPos.y - size),
                    ImVec2(screenPos.x - size, screenPos.y + size),
                    color, 1.5f
                );
                break;
            }

            // Calculate text width and center it
            float textWidth = ImGui::CalcTextSize(infoText).x;
            drawColoredText(ImVec2(screenPos.x - textWidth / 2, screenPos.y - 15), infoText, color);
        };

    // --- DRAW PLAYERS ---
    if (settings::esp::show_players) {
        for (const auto& player : Cache::Players) {
            if (!player) continue;

            auto entity = player->GetObj();
            if (!entity || entity == Localpawn) continue;

            std::string entityName = GetNameById(read<FName>(entity + 0x18).GetDisplayIndex());
            if (entityName.find("BP_CamperFemale") == std::string::npos &&
                entityName.find("BP_CamperMale") == std::string::npos) {
                continue;
            }

            FVector origin = player->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            if (distance > settings::esp::max_render_distance)
                continue;

            POINT screenLeg, screenHead;
            if (!WorldToScreen(cameraCache.POV, FVector{ origin.x, origin.y, origin.z - 70 }, screenLeg) ||
                !WorldToScreen(cameraCache.POV, FVector{ origin.x, origin.y, origin.z + 85 }, screenHead))
                continue;

            int height = screenLeg.y - screenHead.y;
            if (height <= 0) height = 25;
            int width = height / 2;

            const char* survivorType = entityName.find("BP_CamperFemale") != std::string::npos ?
                "Female Survivor" : "Male Survivor";

            float health = 0.75f; // Mock health value

            drawBox(
                screenHead,
                width,
                height,
                ImColor(settings::esp::survivor_color[0], settings::esp::survivor_color[1],
                    settings::esp::survivor_color[2], settings::esp::survivor_color[3]),
                survivorType,
                distance,
                health,
                true, // isPlayer
                settings::esp::show_player_boxes,
                settings::esp::show_player_names,
                settings::esp::show_player_distance,
                settings::esp::show_player_health
            );
        }
    }

    // --- DRAW KILLERS ---
    if (settings::esp::show_killers) {
        for (const auto& killer : Cache::Killers) {
            if (!killer) continue;

            auto entity = killer->GetObj();
            if (!entity || entity == Localpawn) continue;

            FVector origin = killer->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            if (distance > settings::esp::max_render_distance)
                continue;

            POINT screenLeg, screenHead;
            if (!WorldToScreen(cameraCache.POV, FVector{ origin.x, origin.y, origin.z - 70 }, screenLeg) ||
                !WorldToScreen(cameraCache.POV, FVector{ origin.x, origin.y, origin.z + 85 }, screenHead))
                continue;

            int height = screenLeg.y - screenHead.y;
            if (height <= 0) height = 25;
            int width = height / 2;

            drawBox(
                screenHead,
                width,
                height,
                ImColor(settings::esp::killer_color[0], settings::esp::killer_color[1],
                    settings::esp::killer_color[2], settings::esp::killer_color[3]),
                "Killer",
                distance,
                1.0f,
                false, // isPlayer
                settings::esp::show_killer_boxes,
                settings::esp::show_killer_names,
                settings::esp::show_killer_distance,
                settings::esp::show_killer_health
            );
        }
    }

    // --- DRAW PALLETS ---
    if (settings::esp::show_pallets) {
        for (const auto& pallet : Cache::Pallets) {
            if (!pallet) continue;

            auto entity = pallet->GetObj();
            if (!entity) continue;

            std::string entityName = GetNameById(read<FName>(entity + 0x18).GetDisplayIndex());
            if (entityName.find("BP_Pallet") == std::string::npos) continue;

            EPalletState state = static_cast<EPalletState>(read<uint8_t>(entity + 0x3E0));
            if (state == EPalletState::Destroyed) continue;

            FVector origin = pallet->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            ImColor palletColor = ImColor(settings::esp::pallet_color[0], settings::esp::pallet_color[1],
                settings::esp::pallet_color[2], settings::esp::pallet_color[3]);

            drawWorldObject(origin, distance, "Pallet", palletColor, 0);
        }
    }

    // --- DRAW GENERATORS ---
    if (settings::esp::show_generators) {
        for (const auto& gen : Cache::Gens) {
            if (!gen) continue;

            auto entity = gen->GetObj();
            if (!entity) continue;

            FVector origin = gen->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            ImColor genColor = ImColor(settings::esp::generator_color[0], settings::esp::generator_color[1],
                settings::esp::generator_color[2], settings::esp::generator_color[3]);

            drawWorldObject(origin, distance, "Gen", genColor, 0);
        }
    }

    // --- DRAW HATCH ---
    if (settings::esp::show_hatch) {
        for (const auto& hatch : Cache::Hatch) {
            if (!hatch) continue;

            auto entity = hatch->GetObj();
            if (!entity) continue;

            FVector origin = hatch->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            ImColor hatchColor = ImColor(settings::esp::hatch_color[0], settings::esp::hatch_color[1],
                settings::esp::hatch_color[2], settings::esp::hatch_color[3]);

            drawWorldObject(origin, distance, "Hatch", hatchColor, 1);
        }
    }

    // --- DRAW TOTEMS ---
    if (settings::esp::show_totems) {
        for (const auto& totem : Cache::Totem) {
            if (!totem) continue;

            auto entity = totem->GetObj();
            if (!entity) continue;

            FVector origin = totem->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            ImColor totemColor = ImColor(settings::esp::totem_color[0], settings::esp::totem_color[1],
                settings::esp::totem_color[2], settings::esp::totem_color[3]);

            drawWorldObject(origin, distance, "Totem", totemColor, 2);
        }
    }

    // --- DRAW KILLER ITEMS ---
    if (settings::esp::show_killer_items) {
        for (const auto& item : Cache::KillerItems) {
            if (!item) continue;

            auto entity = item->GetObj();
            if (!entity) continue;

            std::string entityName = GetNameById(read<FName>(entity + 0x18).GetDisplayIndex());
            std::string label;

            // Identify killer item type
            if (entityName.find("BP_BearTrap_001_C") != std::string::npos || entityName.find("BearTrap_C") != std::string::npos)
                label = "Bear Trap";
            else if (entityName.find("GroundPortal") != std::string::npos)
                label = "Abyss Portal";
            else if (entityName.find("DreamSnare") != std::string::npos)
                label = "Snare";
            else if (entityName.find("DreamPallet_C") != std::string::npos)
                label = "Dream Pallet";
            else if (entityName.find("WakerObject") != std::string::npos)
                label = "Alarm Clock";
            else if (entityName.find("PhantomTrap") != std::string::npos)
                label = "Phantom Trap";
            else if (entityName.find("ReverseBearTrapRemover") != std::string::npos)
                label = "Jigsaw Box";
            else if (entityName.find("MagicFountain") != std::string::npos)
                label = "Fountain";
            else if (entityName.find("ZombieCharacter") != std::string::npos)
                label = "Zombie";
            else if (entityName.find("SupplyCrate_BP_C") != std::string::npos)
                label = "Vaccine Crate";
            else if (entityName.find("BP_K29SupplyCrate_C") != std::string::npos)
                label = "Spray Crate";
            else if (entityName.find("InfectionRemovalCollectable") != std::string::npos)
                label = "Spray";
            else if (entityName.find("LamentConfiguration") != std::string::npos)
                label = "Lament Configuration";
            else if (entityName.find("OnryoTelevision") != std::string::npos)
                label = "Television";
            else if (entityName.find("BP_K32ItemBox_C") != std::string::npos)
                label = "EMP Printer";
            else if (entityName.find("BP_K32KillerPod_C") != std::string::npos)
                label = "Biopod";
            else if (entityName.find("K33ControlStation") != std::string::npos)
                label = "Control Station";
            else if (entityName.find("K33Turret") != std::string::npos)
                label = "Flame Turret";
            else if (entityName.find("BP_K35KillerTeleportPoint_C") != std::string::npos)
                label = "Hallucination";
            else if (entityName.find("BP_K36TreasureChest_C") != std::string::npos)
                label = "Magic Chest";
            else if (entityName.find("K31Drone") != std::string::npos)
                label = "Drone";
            else
                continue;

            FVector origin = item->GetLocation();
            float distance = calculateDistance(origin, localOrigin);

            ImColor itemColor = ImColor(settings::esp::trap_color[0], settings::esp::trap_color[1],
                settings::esp::trap_color[2], settings::esp::trap_color[3]);

            drawWorldObject(origin, distance, label.c_str(), itemColor, 3);
        }
    }
}
const int PALLET_KEY = 0x20; // Space key by default
float AUTO_PALLET_DISTANCE = 2.5f; // Distance threshold for auto-pallet, can be made configurable


float calculateDistance(const FVector& point1, const FVector& point2) {
    return std::sqrt(
        std::pow(point2.x - point1.x, 2) +
        std::pow(point2.y - point1.y, 2) +
        std::pow(point2.z - point1.z, 2)
    );
}

void AutoPallet() {
    if (!settings::autopal || !Localpawn) return;

    static auto lastPalletTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPalletTime).count() < 500) return;

    FVector nearestPallet{};
    float nearestPalletDistance = FLT_MAX;
    FVector killerPosition{};
    FVector killerVelocity{};
    bool killerFound = false;

    std::lock_guard<std::shared_mutex> lock(entityMutex);

    for (const auto& pallet : Cache::Pallets) {
        if (!pallet || !pallet->GetObj()) continue;

        if (static_cast<EPalletState>(read<uint8_t>(pallet->GetObj() + 0x3E0)) != EPalletState::Up) continue;

        FVector palletLoc = pallet->GetLocation();
        float distance = calculateDistance(palletLoc, localOrigin);

        if (distance < nearestPalletDistance) {
            nearestPallet = palletLoc;
            nearestPalletDistance = distance;
        }
    }

    for (const auto& killer : Cache::Killers) {
        if (!killer || !killer->GetObj()) continue;

        auto killerObj = killer->GetObj();
        auto rootComp = read<uintptr_t>(killerObj + O::AActor::RootComponent);
        if (!rootComp) continue;

        killerPosition = killer->GetLocation();
        killerVelocity = read<FVector>(rootComp + 0x188);
        killerFound = true;
        break;
    }

    if (!killerFound || nearestPallet.IsZero()) return;

    nearestPalletDistance /= 100.0f;
    if (nearestPalletDistance > AUTO_PALLET_DISTANCE) return;

    static const float predictionTime = 0.35f;
    FVector predictedKillerPos = killerPosition + (killerVelocity * predictionTime);

    float killerSpeed = killerVelocity.Length();
    if (killerSpeed < 50.0f) return;

    float killerPlayerDistance = calculateDistance(predictedKillerPos, localOrigin) / 100.0f;
    if (killerPlayerDistance >= 3.0f) return;

    float killerPalletDistance = calculateDistance(predictedKillerPos, nearestPallet) / 100.0f;

    if (killerPalletDistance <= 2.1f && killerPalletDistance >= 0.5f) {
        lastPalletTime = currentTime;

        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = PALLET_KEY;
        SendInput(1, &input, sizeof(INPUT));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }
}

enum class ETotemState : uint8_t {
    Cleansed = 0,
    Dull = 1,
    Hex = 2,
    Boon = 3,
    ETotemState_MAX = 4
};


void RenderAuras() {
    auto IterateObjects = [](const std::vector<AuraCache::Object*>& objects, uint32_t outlineOffset, float auraCol[3]) {
        int count = 0;
        for (const auto& obj : objects) {
            if (!obj) continue;

            auto actor = obj->GetObj();
            if (!actor) continue;

            Aura(actor, outlineOffset, auraCol);
            count++;
        }
        return count;
        };

    int totalObjects = 0;

    int chestCount = IterateObjects(AuraCache::Chests, O::ChestDBDOutline, settings::ChestColor);
    totalObjects += chestCount;

    int genCount = IterateObjects(AuraCache::Gens, O::GeneratorDBDOutline, settings::GenColor);
    totalObjects += genCount;

    int totemCount = 0;
    for (const auto& totem : AuraCache::Totem) {
        if (!totem) continue;

        auto obj = totem->GetObj();
        if (!obj) continue;

        auto totemState = read<ETotemState>(obj + 0x3f8);

        switch (totemState) {
        case ETotemState::Cleansed:
            settings::TotemColor[0] = 0.5f;
            settings::TotemColor[1] = 0.5f;
            settings::TotemColor[2] = 0.5f;
            break;
        case ETotemState::Dull:
            settings::TotemColor[0] = 0.7f;
            settings::TotemColor[1] = 0.7f;
            settings::TotemColor[2] = 1.0f;
            break;
        case ETotemState::Hex:
            settings::TotemColor[0] = 1.0f;
            settings::TotemColor[1] = 0.0f;
            settings::TotemColor[2] = 0.0f;
            break;
        case ETotemState::Boon:
            settings::TotemColor[0] = 0.0f;
            settings::TotemColor[1] = 0.5f;
            settings::TotemColor[2] = 1.0f;
            break;
        default:
            break;
        }

        Aura(obj, O::TotemOutline, settings::TotemColor);
        totemCount++;
    }
    totalObjects += totemCount;

    int palletCount = 0;
    for (const auto& pallet : AuraCache::Pallets) {
        if (!pallet) continue;

        auto obj = pallet->GetObj();
        if (!obj) continue;

        auto palletState = read<EPalletState>(obj + O::APallet::PalletState);

        if (palletState == EPalletState::Destroyed) {
            continue;
        }

        Aura(obj, O::PalletOutline, settings::PalletColor);
        palletCount++;
    }
    totalObjects += palletCount;

    int windowCount = 0;
    for (const auto& window : AuraCache::Windows) {
        if (!window) continue;

        auto obj = window->GetObj();
        if (!obj) continue;

        auto blockComp = read<uintptr_t>(obj + 0x430);
        if (blockComp && read<uintptr_t>(blockComp + 0xd0) != 0) {
            settings::WindowColor[0] = 1.0f;
            settings::WindowColor[1] = 0.0f;
            settings::WindowColor[2] = 0.0f;
        }

        Aura(obj, O::WindowDBDOutline, settings::WindowColor);
        windowCount++;
    }
    totalObjects += windowCount;
}


bool IsKillerItem(const std::string& name) {
    static const std::vector<std::string> killerItems = {
        "BP_BearTrap_001_C", "BearTrap_C", "GroundPortal", "DreamSnare", "DreamPallet_C",
        "WakerObject", "PhantomTrap", "ReverseBearTrapRemover", "MagicFountain",
        "ZombieCharacter", "SupplyCrate_BP_C", "BP_K29SupplyCrate_C",
        "InfectionRemovalCollectable", "LamentConfiguration", "OnryoTelevision",
        "BP_K32ItemBox_C", "BP_K32KillerPod_C", "K33ControlStation", "K33Turret",
        "BP_K35KillerTeleportPoint_C", "BP_K36TreasureChest_C", "K31Drone"
    };

    for (const auto& item : killerItems) {
        if (name.find(item) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void UpdateGameObjects(uintptr_t World, int localId) {
    int objectCount = 0;
 
    auto Levels = read<uintptr_t>(World + O::UWorld::Level);
    if (!Levels) {
        return;
    }

    int levelCount = read<int>(World + O::UWorld::LevelSize);

    for (int i = 0; i < levelCount; i++) {
        auto Level = read<uintptr_t>(Levels + i * 8);
        if (!Level) continue;

        auto Actors = read<uintptr_t>(Level + O::ULevel::ActorArray);
        if (!Actors) continue;

        int actorCount = read<int>(Level + O::ULevel::ActorCount);

        for (int j = 0; j < actorCount; j++) {
            auto Actor = read<uintptr_t>(Actors + j * 8);
            if (!Actor) continue;

            auto nameStruct = read<FName>(Actor + 0x18);
            std::string NameByID = GetNameById(nameStruct.GetDisplayIndex());
            if (NameByID.find("Camera") != std::string::npos) continue;

            uintptr_t rootComponent = read<uintptr_t>(Actor + O::AActor::RootComponent);
            if (!rootComponent) continue;

            FVector origin = read<FVector>(rootComponent + O::USceneComponent::RelativeLocation);

            if (NameByID.find("BP_Camper") != std::string::npos || NameByID.find("BP_Slasher") != std::string::npos) {
                int entId = read<int>(Actor + 0x190);
                if (localId == entId) {
                    continue;
                }

                if (NameByID.find("BP_CamperFemale") != std::string::npos || NameByID.find("BP_CamperMale") != std::string::npos) {
                    Cache::Players.push_back(new Cache::Object(Actor, origin));
                    AuraCache::Players.push_back(new AuraCache::Object(Actor, origin));
                    objectCount++;
                }
                if (NameByID.find("BP_Slasher") != std::string::npos) {
                    Cache::Killers.push_back(new Cache::Object(Actor, origin));
                    AuraCache::Killers.push_back(new AuraCache::Object(Actor, origin));
                    objectCount++;
                }
            }
            else if (NameByID.find("BP_Chest") != std::string::npos) {
                Cache::Chests.push_back(new Cache::Object(Actor, origin));
                AuraCache::Chests.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("WindowStandard") != std::string::npos) {
                Cache::Windows.push_back(new Cache::Object(Actor, origin));
                AuraCache::Windows.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("GeneratorStandard") != std::string::npos || NameByID.find("GeneratorNoPole") != std::string::npos) {
                Cache::Gens.push_back(new Cache::Object(Actor, origin));
                AuraCache::Gens.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("BP_MeatHook") != std::string::npos) {
                Cache::Hooks.push_back(new Cache::Object(Actor, origin));
                AuraCache::Hooks.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("BP_Pallet") != std::string::npos) {
                Cache::Pallets.push_back(new Cache::Object(Actor, origin));
                AuraCache::Pallets.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("BP_Totem") != std::string::npos) {
                Cache::Totem.push_back(new Cache::Object(Actor, origin));
                AuraCache::Totem.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (NameByID.find("BP_Hatch01_") != std::string::npos) {
                Cache::Hatch.push_back(new Cache::Object(Actor, origin));
                AuraCache::Hatch.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
            else if (IsKillerItem(NameByID)) {
                Cache::KillerItems.push_back(new Cache::Object(Actor, origin));
                AuraCache::KillerItems.push_back(new AuraCache::Object(Actor, origin));
                objectCount++;
            }
        }
    }
}

void UpdatePlayerPositions(std::vector<Cache::Object*>& objects) {
    int updatedCount = 0;
    for (auto obj : objects) {
        if (!obj) continue;
        uintptr_t actor = obj->GetObj();
        if (!actor) continue;

        uintptr_t rootComponent = read<uintptr_t>(actor + O::AActor::RootComponent);
        if (!rootComponent) continue;

        FVector newLocation = read<FVector>(rootComponent + O::USceneComponent::RelativeLocation);
        obj->location = newLocation;
        updatedCount++;
    }

    if (!objects.empty()) {
    }
}

void UpdateStaticObjectPositions(std::vector<Cache::Object*>& objects) {
    int updatedCount = 0;
    for (auto obj : objects) {
        if (!obj) continue;
        uintptr_t actor = obj->GetObj();
        if (!actor) continue;

        uintptr_t rootComponent = read<uintptr_t>(actor + O::AActor::RootComponent);
        if (!rootComponent) continue;

        FVector newLocation = read<FVector>(rootComponent + O::USceneComponent::RelativeLocation);
        obj->location = newLocation;
        updatedCount++;
    }
}

void UpdateObjectPositions() {
    UpdatePlayerPositions(Cache::Players);
    UpdatePlayerPositions(Cache::Killers);
    UpdateStaticObjectPositions(Cache::Chests);
    UpdateStaticObjectPositions(Cache::Windows);
    UpdateStaticObjectPositions(Cache::Gens);
    UpdateStaticObjectPositions(Cache::Hooks);
    UpdateStaticObjectPositions(Cache::Pallets);
    UpdateStaticObjectPositions(Cache::Totem);
    UpdateStaticObjectPositions(Cache::Hatch);
    UpdateStaticObjectPositions(Cache::KillerItems);
}

void SyncObjectType(std::vector<Cache::Object*>& source, std::vector<AuraCache::Object*>& target) {
    int syncCount = 0;
    for (size_t i = 0; i < source.size() && i < target.size(); i++) {
        if (source[i] && target[i]) {
            target[i]->location = source[i]->GetLocation();
            syncCount++;
        }
    }
}

void SynchronizeCaches() {

    for (size_t i = 0; i < Cache::Players.size() && i < AuraCache::Players.size(); i++) {
        if (Cache::Players[i] && AuraCache::Players[i]) {
            AuraCache::Players[i]->location = Cache::Players[i]->GetLocation();
        }
    }

    for (size_t i = 0; i < Cache::Killers.size() && i < AuraCache::Killers.size(); i++) {
        if (Cache::Killers[i] && AuraCache::Killers[i]) {
            AuraCache::Killers[i]->location = Cache::Killers[i]->GetLocation();
        }
    }

    SyncObjectType(Cache::Pallets, AuraCache::Pallets);
    SyncObjectType(Cache::Gens, AuraCache::Gens);
    SyncObjectType(Cache::Chests, AuraCache::Chests);
    SyncObjectType(Cache::Windows, AuraCache::Windows);
    SyncObjectType(Cache::Hooks, AuraCache::Hooks);
    SyncObjectType(Cache::Totem, AuraCache::Totem);
    SyncObjectType(Cache::Hatch, AuraCache::Hatch);
    SyncObjectType(Cache::KillerItems, AuraCache::KillerItems);
}

float GetKillerBlindnessValue(uintptr_t killerAddress) {
    if (!killerAddress) return 0.0f;

    uintptr_t blindableComponent = read<uintptr_t>(killerAddress + 0xe38); // _blindableComponent offset
    if (!blindableComponent) return 0.0f;

    bool isBlinded = read<bool>(blindableComponent + 0x150); // _isBlinded offset

    auto isBeingBlindedState = read<uint8_t>(blindableComponent + 0x158); // _isBeingBlinded offset

    uintptr_t blindnessCurve = read<uintptr_t>(killerAddress + 0x1e48); // BlindnessCurve offset

    if (isBlinded) return 1.0f;

    if (isBeingBlindedState != 0) return 0.5f;

    return 0.0f;
}

void PrintKillerBlindnessStatus() {
    if (Cache::Killers.empty()) return;

    for (auto killer : Cache::Killers) {
        if (!killer) continue;

        float blindnessValue = GetKillerBlindnessValue(killer->GetObj());

        int blindnessPercent = static_cast<int>(blindnessValue * 100.0f);

        std::string progressBar = "[";
        int barLength = 20;
        int filledLength = static_cast<int>(blindnessValue * barLength);

        for (int i = 0; i < barLength; i++) {
            if (i < filledLength)
                progressBar += "=";
            else
                progressBar += " ";
        }
        progressBar += "]";
        printf("Killer Blindness: %d%% %s\r", blindnessPercent, progressBar.c_str());
    }
}



void game_loop() {
    static uint64_t lastFullCacheUpdate = 0;
    static uint64_t lastPositionUpdate = 0;
    uint64_t currentTime = GetTickCount64();

    bool shouldUpdateFullCache = (currentTime - lastFullCacheUpdate) > 2000;
    bool shouldUpdatePositions = (currentTime - lastPositionUpdate) > 30;

    auto World = read<uintptr_t>(virtualaddy + O::GWorld);
    if (!World) {
        printf("[DEBUG] World is null!\n");
        return;
    }

    auto WorldName = read<FName>(World + 0x18);
    std::string worldNameStr = GetNameById(WorldName.GetDisplayIndex());
    if (worldNameStr.find("Lobby") != std::string::npos || (GetAsyncKeyState(VK_F2) & 1)) {
        return;
    }

    auto gameInstance = read<uintptr_t>(World + O::UWorld::OwningGameInstance);
    auto localPlayers = read<uintptr_t>(gameInstance + O::UGameInstance::LocalPlayers);
    auto localPlayer = read<uintptr_t>(localPlayers);
    auto playerController = read<uintptr_t>(localPlayer + O::UPlayer::PlayerController);
    auto cameraManager = read<uintptr_t>(playerController + O::APlayerController::PlayerCameraManager);

    cameraCache = read<FCameraCacheEntry>(cameraManager + O::APlayerCameraManager::CameraCachePrivate);

    static float fovValue = settings::custom_fov;

    if (GetAsyncKeyState('U') & 0x8000) settings::custom_fov += 1.0f;
    if (GetAsyncKeyState('I') & 0x8000) settings::custom_fov -= 1.0f;

    uintptr_t fovAddress = cameraManager + 0x2C0;

    if (settings::custom_fov_enabled || fovValue != settings::custom_fov) {
        fovValue = settings::custom_fov;
        write<float>(fovAddress, fovValue);
    }

    cameraCache = read<FCameraCacheEntry>(cameraManager + O::APlayerCameraManager::CameraCachePrivate);
    if (settings::custom_fov_enabled) {
        cameraCache.POV.FOV = fovValue;
    }

    Localpawn = read<uintptr_t>(playerController + O::APlayerController::AcknowledgedPawn);

    if (!Localpawn) {
        return;
    }

    localId = read<int>(Localpawn + 0x190);
    uintptr_t localRootComponent = read<uintptr_t>(Localpawn + O::AActor::RootComponent);
    localOrigin = read<FVector>(localRootComponent + O::USceneComponent::RelativeLocation);


    if (shouldUpdateFullCache) {
        for (auto obj : Cache::Players) delete obj;
        for (auto obj : Cache::Killers) delete obj;
        for (auto obj : Cache::Chests) delete obj;
        for (auto obj : Cache::Pallets) delete obj;
        for (auto obj : Cache::Gens) delete obj;
        for (auto obj : Cache::Hooks) delete obj;
        for (auto obj : Cache::Windows) delete obj;
        for (auto obj : Cache::KillerItems) delete obj;
        for (auto obj : Cache::Hatch) delete obj;
        for (auto obj : Cache::Totem) delete obj;

        Cache::Players.clear();
        Cache::Killers.clear();
        Cache::Chests.clear();
        Cache::Pallets.clear();
        Cache::Gens.clear();
        Cache::Hooks.clear();
        Cache::Windows.clear();
        Cache::KillerItems.clear();
        Cache::Hatch.clear();
        Cache::Totem.clear();

        for (auto obj : AuraCache::Players) delete obj;
        for (auto obj : AuraCache::Killers) delete obj;
        for (auto obj : AuraCache::Chests) delete obj;
        for (auto obj : AuraCache::Pallets) delete obj;
        for (auto obj : AuraCache::Gens) delete obj;
        for (auto obj : AuraCache::Hooks) delete obj;
        for (auto obj : AuraCache::Windows) delete obj;
        for (auto obj : AuraCache::KillerItems) delete obj;
        for (auto obj : AuraCache::Hatch) delete obj;
        for (auto obj : AuraCache::Totem) delete obj;

        AuraCache::Players.clear();
        AuraCache::Killers.clear();
        AuraCache::Chests.clear();
        AuraCache::Pallets.clear();
        AuraCache::Gens.clear();
        AuraCache::Hooks.clear();
        AuraCache::Windows.clear();
        AuraCache::KillerItems.clear();
        AuraCache::Hatch.clear();
        AuraCache::Totem.clear();

        UpdateGameObjects(World, localId);
        lastFullCacheUpdate = currentTime;
    }

    if (shouldUpdatePositions) {
        UpdateObjectPositions();
        SynchronizeCaches();
        lastPositionUpdate = currentTime;

        if (settings::show_killer_blindness) {
            PrintKillerBlindnessStatus();
        }
    }

    if (settings::aura_enabled) {
        for (auto player : Cache::Players) {
            if (!player) continue;
            Aura(player->GetObj(), O::SurvivorOutline, settings::PlayerColor);
        }

        for (auto killer : Cache::Killers) {
            if (!killer) continue;
            Aura(killer->GetObj(), O::KillerOutline, settings::KillerColor);
        }

        RenderAuras();
    }

    if (settings::esp::enable) {
        DrawESP();
    }

    if (settings::autopal) {
        AutoPallet();
    }
}

void render_menu() {
    static float colorTimer = 0.0f;
    static int currentColorIndex = 0;
    static int nextColorIndex = 1;
    ImVec4 colorArray[6] = {
        ImVec4(0.63f, 0.13f, 0.94f, 1.0f),
        ImVec4(0.0f, 0.0f, 1.0f, 1.0f),
        ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        ImVec4(1.0f, 0.0f, 1.0f, 1.0f),
        ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
    };

    colorTimer += ImGui::GetIO().DeltaTime * 0.5f;
    if (colorTimer >= 1.0f) {
        colorTimer = 0.0f;
        currentColorIndex = nextColorIndex;
        nextColorIndex = (nextColorIndex + 1) % 6;
    }

    ImVec4 currentColor = colorArray[currentColorIndex];
    ImVec4 nextColor = colorArray[nextColorIndex];
    ImVec4 interpolatedColor = ImVec4(
        currentColor.x + (nextColor.x - currentColor.x) * colorTimer,
        currentColor.y + (nextColor.y - currentColor.y) * colorTimer,
        currentColor.z + (nextColor.z - currentColor.z) * colorTimer,
        currentColor.w + (nextColor.w - currentColor.w) * colorTimer
    );

    if (GetAsyncKeyState(VK_INSERT) & 1)
        settings::show_menu = !settings::show_menu;

    if (!settings::show_menu)
        return;

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 10.0f;
    style.WindowRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.Colors[ImGuiCol_Border] = interpolatedColor;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.40f, 0.60f, 1.0f);

    ImGui::SetNextWindowSize({ 650, 450 });
    ImGui::Begin("VWARE", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

    ImGui::BeginChild("Tabs", ImVec2(150, 0), true);
    const char* tabs[] = { "ESP", "Auras", "Auto Features", "FOV", "Misc", "Exit" };
    for (int i = 0; i < IM_ARRAYSIZE(tabs); i++) {
        if (ImGui::Selectable(tabs[i], settings::tab == i))
            settings::tab = i;
        ImGui::Spacing();
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("MainContent", ImVec2(0, 0), true);

    switch (settings::tab) {
    case 0: {
        ImGui::Checkbox("Enable ESP", &settings::esp::enable);
        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::CollapsingHeader("ESP Elements", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::CollapsingHeader("Player ESP Options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(10.0f);

                ImGui::Text("Survivor Options:");
                ImGui::Checkbox("Show Survivors", &settings::esp::show_players);

                if (settings::esp::show_players) {
                    ImGui::Indent(10.0f);
                    ImGui::Checkbox("Show Boxes", &settings::esp::show_player_boxes);
                    ImGui::Checkbox("Show Names", &settings::esp::show_player_names);
                    ImGui::Checkbox("Show Distance", &settings::esp::show_player_distance);
                    ImGui::Checkbox("Show Health", &settings::esp::show_player_health);
                    ImGui::ColorEdit4("Survivor Color", settings::esp::survivor_color);
                    ImGui::Unindent(10.0f);
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Text("Killer Options:");
                ImGui::Checkbox("Show Killers", &settings::esp::show_killers);

                if (settings::esp::show_killers) {
                    ImGui::Indent(10.0f);
                    ImGui::Checkbox("Show Boxes", &settings::esp::show_killer_boxes);
                    ImGui::Checkbox("Show Names", &settings::esp::show_killer_names);
                    ImGui::Checkbox("Show Distance", &settings::esp::show_killer_distance);
                    ImGui::Checkbox("Show Health", &settings::esp::show_killer_health);
                    ImGui::ColorEdit4("Killer Color", settings::esp::killer_color);
                    ImGui::Unindent(10.0f);
                }

                ImGui::Unindent(10.0f);
            }
            ImGui::Checkbox("Show Pallets", &settings::esp::show_pallets);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##PalletColor", settings::esp::pallet_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Generators", &settings::esp::show_generators);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##GenColor", settings::esp::generator_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Hatch", &settings::esp::show_hatch);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##HatchColor", settings::esp::hatch_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Totems", &settings::esp::show_totems);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##TotemColor", settings::esp::totem_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Killer Items", &settings::esp::show_killer_items);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##TrapColor", settings::esp::trap_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Chests", &settings::esp::show_chests);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##ChestColor", settings::esp::chest_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Hooks", &settings::esp::show_hooks);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##HookColor", settings::esp::hook_color, ImGuiColorEditFlags_NoInputs);

            ImGui::Checkbox("Show Windows", &settings::esp::show_windows);
            ImGui::SameLine(160);
            ImGui::ColorEdit4("##WindowColor", settings::esp::window_color, ImGuiColorEditFlags_NoInputs);

            ImGui::SliderFloat("Max Render Distance", &settings::esp::max_render_distance, 10.0f, 300.0f, "%.1f m");
            ImGui::Unindent(10.0f);
        }

        if (ImGui::CollapsingHeader("Visual Settings")) {
            ImGui::Indent(10.0f);
            ImGui::Checkbox("Filled Boxes", &settings::esp::filled_boxes);
            ImGui::SliderFloat("Box Transparency", &settings::esp::box_alpha, 0.0f, 1.0f);
            ImGui::SliderFloat("Outline Thickness", &settings::esp::outline_thickness, 0.5f, 3.0f);
            ImGui::Checkbox("Show Corner Lines", &settings::esp::show_corner_lines);
            ImGui::SliderFloat("Corner Line Length", &settings::esp::corner_line_length, 3.0f, 15.0f);
            ImGui::Checkbox("Rainbow Text", &settings::esp::rainbow_text);
            ImGui::Checkbox("Show Info Panel", &settings::esp::show_info_panel);
            ImGui::Unindent(10.0f);
        }

        if (ImGui::CollapsingHeader("Information Display")) {
            ImGui::Indent(10.0f);
            ImGui::Checkbox("Show Distance", &settings::esp::show_distance);
            ImGui::Checkbox("Show Health Bar", &settings::esp::show_health_bar);
            ImGui::Checkbox("Show Names", &settings::esp::show_names);
            ImGui::Checkbox("Text Shadow", &settings::esp::text_shadow);
            ImGui::Unindent(10.0f);
        }

        break;
    }

    case 1: {
        ImGui::Checkbox("Enable Auras", &settings::aura_enabled);
        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Aura Colors");
        ImGui::ColorEdit3("Players", settings::PlayerColor);
        ImGui::ColorEdit3("Killers", settings::KillerColor);
        ImGui::ColorEdit3("Pallets", settings::PalletColor);
        ImGui::ColorEdit3("Generators", settings::GenColor);
        ImGui::ColorEdit3("Windows", settings::WindowColor);
        ImGui::ColorEdit3("Chests", settings::ChestColor);
        ImGui::ColorEdit3("Totems", settings::TotemColor);
        ImGui::ColorEdit3("Hooks", settings::HookColor);

        break;
    }

    case 2: { // Auto Features Tab
        ImGui::Text("Auto Pallet Settings");
        ImGui::Checkbox("Enable Auto Pallet", &settings::autopal);
        ImGui::SliderFloat("Pallet Distance Threshold", &settings::AUTO_PALLET_DISTANCE, 1.0f, 5.0f, "%.1fm");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Skillcheck Settings");
        ImGui::Checkbox("Enable Auto Skillcheck", &settings::auto_skillcheck);
        ImGui::Checkbox("Perfect Skillchecks Only", &settings::perfect_skillcheck);
        ImGui::SliderFloat("Skillcheck Delay", &settings::skillcheck_delay, 0.0f, 100.0f, "%.1f ms");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


        break;
    }

    case 3: { // FOV Tab
        ImGui::Text("Field of View Settings");
        ImGui::Checkbox("Enable Custom FOV", &settings::custom_fov_enabled);
        ImGui::SliderFloat("FOV Value", &settings::custom_fov, 70.0f, 200.0f, "%.1f");
        ImGui::Spacing();
        ImGui::Text("You can also adjust FOV in-game with U/I keys");

        break;
    }

    case 4: { // Misc Tab
        ImGui::Text("Miscellaneous Settings");
        ImGui::Separator();

        ImGui::Checkbox("Show Killer Blindness Status", &settings::show_killer_blindness);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Debug Information");

        ImGui::Text("Current Date/Time: 2025-03-04 13:42:58");
        ImGui::Text("User: JesusScripts");

        break;
    }

    case 5: { // Exit Tab
        ImGui::Text("Are you sure you want to exit?");
        ImGui::Spacing();

        if (ImGui::Button("Exit Cheat", { 130, 30 }))
            exit(0);

        ImGui::SameLine();

        if (ImGui::Button("Cancel", { 130, 30 }))
            settings::tab = 0;

        break;
    }
    }

    ImGui::EndChild();
    ImGui::End();
}

HWND get_process_wnd(uint32_t pid)
{
    std::pair<HWND, uint32_t> params = { 0, pid };
    BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL {
        auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
        uint32_t processid = 0;
        if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second) {
            SetLastError((uint32_t)-1);
            pparams->first = hwnd;
            return FALSE;
        }
        return TRUE;
        }, (LPARAM)&params);
    if (!bresult && GetLastError() == -1 && params.first)
        return params.first;
    return 0;
}
WPARAM render_loop()
{
    //create_overlay(); // Create the overlay window

    static RECT old_rc;
    ZeroMemory(&messager, sizeof(MSG));
    while (messager.message != WM_QUIT)
    {
        if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&messager);
            DispatchMessage(&messager);
        }
        if (game_wnd == NULL)
            exit(0);
        HWND active_wnd = GetForegroundWindow();
        if (active_wnd == game_wnd)
        {
            HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
            SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else
        {
            game_wnd = get_process_wnd(processID);
            Sleep(250);
        }
        RECT rc;
        POINT xy;
        ZeroMemory(&rc, sizeof(RECT));
        ZeroMemory(&xy, sizeof(POINT));
        rc.left = xy.x;
        rc.top = xy.y;
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f / 60.0f;
        POINT p;
        GetCursorPos(&p);
        io.MousePos.x = p.x - xy.x;
        io.MousePos.y = p.y - xy.y;
        if (GetAsyncKeyState(0x1))
        {
            io.MouseDown[0] = true;
            io.MouseClicked[0] = true;
            io.MouseClickedPos[0].x = io.MousePos.x;
            io.MouseClickedPos[0].y = io.MousePos.y;
        }
        else
        {
            io.MouseDown[0] = false;
        }
        if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
        {
            old_rc = rc;
            settings::width = rc.right;
            settings::height = rc.bottom;
            p_params.BackBufferWidth = settings::width;
            p_params.BackBufferHeight = settings::height;
            SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
            p_device->Reset(&p_params);
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        game_loop();

        render_menu();
        ImGui::EndFrame();
        p_device->SetRenderState(D3DRS_ZENABLE, false);
        p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
        p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (p_device->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            p_device->EndScene();
        }
        HRESULT result = p_device->Present(0, 0, 0, 0);
        if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            p_device->Reset(&p_params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (p_device != 0)
    {
        p_device->EndScene();
        p_device->Release();
    }
    if (p_object != 0)
        p_object->Release();
    DestroyWindow(my_wnd);
    return messager.wParam;
}