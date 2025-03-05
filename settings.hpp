#pragma once
#include "imgui/imgui.h"
#include <Windows.h>

// SETTINGS HEADER
namespace crosshair
{
    inline float crosshair_thickness = 2.0f;
    inline float crosshair_radius = 3.0f;
    inline float crosshair_length = 10.0f;
    inline float crosshair_color = IM_COL32(7, 255, 255, 255);
    inline bool Crosshair = true;
}

namespace settings
{
    // Window metrics
    inline int width = GetSystemMetrics(SM_CXSCREEN);
    inline int height = GetSystemMetrics(SM_CYSCREEN);
    inline int screen_center_x = width / 2;
    inline int screen_center_y = height / 2;

    // Menu settings
    inline bool show_menu = true;
    inline int tab = 0;

    // Crosshair settings (global)
    inline float crosshair_thickness = 2.0f;
    inline float crosshair_radius = 3.0f;
    inline float crosshair_length = 10.0f;
    inline bool Crosshair = false;

    // Auto features
    inline bool autopal = false;
    inline float AUTO_PALLET_DISTANCE = 2.8f;
    inline bool auto_skillcheck = false;
    inline bool perfect_skillcheck = false;
    inline float skillcheck_delay = 0.0f;

    // FOV settings
    inline bool custom_fov_enabled = false;
    inline float custom_fov = 150.0f;

    // Aura colors
    inline float ChestColor[3] = { 1.0f, 0.95f, 0.2f };       // Brighter yellow for chests
    inline float GenColor[3] = { 0.0f, 0.9f, 1.0f };          // Cyan/turquoise for generators
    inline float TotemColor[3] = { 1.0f, 0.4f, 1.0f };        // Vibrant magenta for default totem color
    inline float PalletColor[3] = { 1.0f, 0.4f, 0.0f };       // Bright orange for pallets
    inline float WindowColor[3] = { 0.2f, 1.0f, 0.4f };       // Lime green for windows
    inline float PlayerColor[3] = { 0.0f, 1.0f, 0.0f };       // Vibrant green for players
    inline float KillerColor[3] = { 1.0f, 0.1f, 0.1f };       // Bright red for killers
    inline float HookColor[3] = { 0.8f, 0.2f, 0.2f };         // Red for hooks

    // Aura system
    inline bool aura_enabled = true;

    // Blindness notification
    inline bool show_killer_blindness = true;

    namespace aimbot
    {
        inline bool enable = false;
        inline bool show_fov = false;
        inline float fov = 150;
        inline float smoothness = 5;
        inline static const char* aimkey[] = { "Left Mouse Button", "Right Mouse Button" };
        inline static int current_aimkey = 1;
        inline static int current_key = VK_RBUTTON;
    }

    namespace visuals {
        inline bool enable = true;
        inline bool show_players = true;
        inline bool show_killers = true;
        inline bool show_generators = true;
        inline bool show_pallets = true;
        inline bool show_totems = true;
        inline bool show_chests = true;
        inline bool show_killer_items = true;
        inline bool show_hooks = true;
        inline bool show_windows = true;
        inline bool show_hatch = true;
        inline float render_distance = 100.0f;
        inline float refresh_rate = 30.0f;
    }

    namespace esp {
        // Main toggles
        inline bool enable = true;

        // Players
        inline bool show_players = true;
        inline bool show_player_boxes = false;
        inline bool show_player_names = false;
        inline bool show_player_distance = true;
        inline bool show_player_health = true;

        // Killers
        inline bool show_killers = true;
        inline bool show_killer_boxes = false;
        inline bool show_killer_names = false;
        inline bool show_killer_distance = true;
        inline bool show_killer_health = true;

        // Objects
        inline bool show_pallets = true;
        inline bool show_generators = true;
        inline bool show_hatch = true;
        inline bool show_totems = true;
        inline bool show_killer_items = true;
        inline bool show_chests = true;
        inline bool show_hooks = true;
        inline bool show_windows = true;

        // Visual settings
        inline bool filled_boxes = true;
        inline float box_alpha = 0.3f;
        inline float outline_thickness = 1.5f;
        inline bool show_corner_lines = true;
        inline float corner_line_length = 8.0f;

        // Visual effects
        inline bool rainbow_text = false;
        inline bool show_info_panel = true;

        // Information display
        inline bool show_distance = true;
        inline bool show_health_bar = true;
        inline bool show_names = true;
        inline bool text_shadow = true;
        inline float max_render_distance = 100.0f; // Maximum distance to render ESP elements

        // Colors (RGBA format)
        inline float survivor_color[4] = { 0.0f, 1.0f, 0.4f, 1.0f };
        inline float killer_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
        inline float pallet_color[4] = { 0.78f, 0.7f, 0.2f, 1.0f };
        inline float generator_color[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
        inline float hatch_color[4] = { 1.0f, 0.65f, 0.0f, 1.0f };
        inline float totem_color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
        inline float trap_color[4] = { 1.0f, 0.33f, 0.33f, 1.0f };
        inline float chest_color[4] = { 1.0f, 0.8f, 0.0f, 1.0f };
        inline float hook_color[4] = { 0.8f, 0.2f, 0.2f, 1.0f };
        inline float window_color[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
    }
}

namespace drawing {
    // Fallback debug rectangle color (green) if no ESP objects are drawn.
    inline const ImU32 fallbackColor = IM_COL32(0, 255, 0, 255);
}

namespace Features
{
    inline bool FPS = true;
    inline bool isVSyncEnabled = true;
    inline bool Watermark = true;
    inline bool RenderCount = false;
    inline bool Debug = false;
}

namespace Exploits
{
    inline bool AimInAir = false;
    inline bool no_recoil = false;
}