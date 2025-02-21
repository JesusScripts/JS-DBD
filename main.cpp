#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "global.h"
#include <thread>
#include <iostream>
#include "driver.hpp"
#include "offset.h"
#include "utils.h"
#include "mem.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

HWND hGameWnd;
HWND hOverlayWnd;
RECT wndRect;

void checkKeys() {
    while (true)
    {
        if (rendering)
        {
            if (GetAsyncKeyState(VK_F1) & 1)
                if (showmenu)
                    esp = !esp;

            if (GetAsyncKeyState(VK_F2) & 1)
                if (showmenu)
                    infoEsp = !infoEsp;

            if (GetAsyncKeyState(VK_F3) & 1)
                if (showmenu)
                    drawMisc = !drawMisc;

            if (GetAsyncKeyState(VK_F4) & 1)
                if (showmenu)
                    espLine = !espLine;

            if (GetAsyncKeyState(VK_F5) & 1)
                if (showmenu)
                    name = !name;

            if (GetAsyncKeyState(VK_F6) & 1)
                if (showmenu)
                    box = !box;

            if (GetAsyncKeyState(VK_F7) & 1)
                if (showmenu)
                    corneredBox = !corneredBox;

            if (GetAsyncKeyState(VK_F8) & 1)
                if (showmenu)
                    freecamera = !freecamera;

        }
        if (GetAsyncKeyState(VK_INSERT) & 1)
            showmenu = !showmenu;
    }
}

cameraMultipliers generateMultipliers(float yaw, float pitch) {

    cameraMultipliers multipliers;


    multipliers.yawMultiplierX = (-abs(yaw) + 90) / 90;

    multipliers.yawMultiplierY = abs(yaw);
    if (multipliers.yawMultiplierY > 90) {
        multipliers.yawMultiplierY -= 90;
        multipliers.yawMultiplierY = 90 - multipliers.yawMultiplierY;

    }

    if (abs(yaw) / yaw == -1) {
        multipliers.yawMultiplierY *= -1;
    }
    multipliers.yawMultiplierY *= -1;
    multipliers.yawMultiplierY /= 90;



    multipliers.pitchMultiplier = -1 * abs(pitch) / 90 * abs(pitch) / pitch;

    return multipliers;
}

//void unlockCamera() {
//    mem::write_physical((PVOID)(cameraManager + 0x3334), &float(-179.9), sizeof(float));
//    mem::write_physical((PVOID)(cameraManager + 0x3338), &float(179.9), sizeof(float));
//}

//void lockCamera() {
//    mem::write_physical((PVOID)(cameraManager + 0x3334), &float(-40), sizeof(float));
//    mem::write_physical((PVOID)(cameraManager + 0x3338), &float(50), sizeof(float));
//}

void updateCamera() {

    float multiplier;
    float yaw, pitch;
    cameraMultipliers multipliers;
    float step = 15;
    float stepY;
    float stepX;

    bool cameraLock = true;

    Sleep(1000);
    while (true) {

        multiplier = 1;

        yaw = cameraCache.POV.Rotation.y;
        pitch = cameraCache.POV.Rotation.x;


        if (!freecamera) {
            lastCameraPos.x = cameraCache.POV.Location.x;
            lastCameraPos.y = cameraCache.POV.Location.y;
            lastCameraPos.z = cameraCache.POV.Location.z;
            if (!cameraLock) {
            //    lockCamera();
                cameraLock = !cameraLock;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }


        else if (freecamera) {

            if (cameraLock) {
             //   unlockCamera();
                cameraLock = !cameraLock;
            }

            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                multiplier = 2;
            }

            if (GetAsyncKeyState('W') & 0x8000) {
                multipliers = generateMultipliers(yaw, pitch);
                lastCameraPos.x += 15 * multiplier * multipliers.yawMultiplierX;
                lastCameraPos.y -= 15 * multiplier * multipliers.yawMultiplierY;
                lastCameraPos.z -= 15 * multiplier * multipliers.pitchMultiplier;
            }

            if (GetAsyncKeyState('S') & 0x8000) {
                multipliers = generateMultipliers(yaw, pitch);
                lastCameraPos.x -= 15 * multiplier * multipliers.yawMultiplierX;
                lastCameraPos.y += 15 * multiplier * multipliers.yawMultiplierY;
                lastCameraPos.z += 15 * multiplier * multipliers.pitchMultiplier;
            }

            if (GetAsyncKeyState('A') & 0x8000) {
                yaw -= 90;
                if (yaw < -180)
                    yaw = 180 - (-180 - yaw);
                multipliers = generateMultipliers(yaw, pitch);
                lastCameraPos.x += 15 * multiplier * multipliers.yawMultiplierX;
                lastCameraPos.y -= 15 * multiplier * multipliers.yawMultiplierY;

            }

            if (GetAsyncKeyState('D') & 0x8000) {
                yaw += 90;
                if (yaw > 180)
                    yaw = (180 - (yaw - 180)) * -1;
                multipliers = generateMultipliers(yaw, pitch);
                lastCameraPos.x += 15 * multiplier * multipliers.yawMultiplierX;
                lastCameraPos.y -= 15 * multiplier * multipliers.yawMultiplierY;
            }

            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                lastCameraPos.z += 15 * multiplier;
            }

            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                lastCameraPos.z -= 15 * multiplier;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        }

    }
}

void setCameraCash() {
    Sleep(1250);
    constexpr size_t OffsetPOV = 0x10;
    constexpr size_t OffsetLocation = OffsetPOV + offsetof(FMinimalViewInfo, Location);


    while (true) {

        if (!freecamera) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        try {
            mem::write_physical((PVOID)(cameraManager + offsets::CameraCachePrivate + OffsetLocation), &lastCameraPos, sizeof(FVector));
        }
        catch (...) {
            continue;
        }

    }

}


void setupEntities() {
    std::vector<Entity> players = {};
    std::vector<Entity> miscs = {};
    int localId;

    Entity entity;
    uintptr_t actor;
    uintptr_t playerState;
    int entId;
    uintptr_t root_component;


    while (true) {


        try {
            uWorld = read<uintptr_t>(process_base + offsets::GWorld);
            gameInstance = read<uintptr_t>(uWorld + offsets::OwningGameInstance);
            localPlayerPtr = read<uintptr_t>(gameInstance + offsets::LocalPlayers);
            localPlayer = read<uintptr_t>(localPlayerPtr);
            playerController = read<uintptr_t>(localPlayer + offsets::PlayerController);

            persistentLevel = read<uintptr_t>(uWorld + offsets::PersistentLevel);
            actorsArray = read<uintptr_t>(persistentLevel + offsets::ActorArray);
            actorsCount = read<int>(persistentLevel + offsets::ActorCount);
            Localpawn = read<uintptr_t>(playerController + offsets::AcknowledgedPawn);
            localId = read<int>(Localpawn + 0x190);
            cameraManager = read<uintptr_t>(playerController + offsets::PlayerCameraManager);
            cameraCache = read<FCameraCacheEntry>(cameraManager + offsets::CameraCachePrivate);


            for (int i = 0; i < actorsCount; ++i) {

                entity = {};

                actor = read<uintptr_t>(actorsArray + i * 0x8);

                playerState = read<uintptr_t>(actor + offsets::PlayerState);

                if (!actor)
                    continue;

                entity.id = read<int>(actor + offsets::ActorID);


                if (GetNameById(entity.id).find("BP_CamperMale") != std::string::npos || GetNameById(entity.id).find("BP_CamperFemale") != std::string::npos || GetNameById(entity.id).find("BP_Slasher") != std::string::npos)
                {

                    entId = read<int>(actor + 0x190);
                    root_component = read<uintptr_t>(actor + offsets::RootComponent);

                    if (localId == entId) {
                        localOrigin = read<FVector>(root_component + offsets::RelativeLocation);
                        if (GetNameById(entity.id).find("BP_Slasher") != std::string::npos)
                            localIsSlasher = true;
                        else
                            localIsSlasher = false;
                        continue;
                    }


                    entity.origin = read<FVector>(root_component + offsets::RelativeLocation);


                    entity.distTo = ToMeters(localOrigin.DistTo(entity.origin)) - 2;

                    if (GetNameById(entity.id).find("BP_CamperMale") != std::string::npos)
                        entity.nameId = 1;

                    if (GetNameById(entity.id).find("BP_CamperFemale") != std::string::npos)
                        entity.nameId = 2;

                    if (GetNameById(entity.id).find("BP_Slasher") != std::string::npos)
                        entity.nameId = 3;

                    players.push_back(entity);

                }
                else if ((GetNameById(entity.id).find("Pallet") != std::string::npos) || (GetNameById(entity.id).find("BP_IND_ExitGate") != std::string::npos)) {

                    entId = read<int>(actor + 0x190);
                    root_component = read<uintptr_t>(actor + offsets::RootComponent);

                    entity.origin = read<FVector>(root_component + offsets::RelativeLocation);
                    entity.distTo = ToMeters(localOrigin.DistTo(entity.origin)) - 2;

                    if (GetNameById(entity.id).find("Pallet") != std::string::npos)
                        entity.nameId = 4;

                    if (GetNameById(entity.id).find("BP_IND_ExitGate") != std::string::npos)
                        entity.nameId = 5;


                    miscs.push_back(entity);
                }
                else
                    continue;

            }
        }

        catch (...) {
            misc2Draw = miscs;
            players2Draw = players;
            miscs.clear();
            players.clear();
            continue;
        }

        misc2Draw = miscs;
        players2Draw = players;
        miscs.clear();
        players.clear();


    }
}

void drawESPmisc(Nvidia* overlay) {


    const char* compareStrPallet = "Pallet";
    const char* compareStrGate = "Gate";

    char str[256];



    for (Entity entity : misc2Draw) {

        POINT lowPoint = WorldToScreen(cameraCache.POV, { entity.origin.x, entity.origin.y , entity.origin.z - 70 });
        POINT midPoint = WorldToScreen(cameraCache.POV, { entity.origin.x, entity.origin.y , entity.origin.z });
        POINT highPoint = WorldToScreen(cameraCache.POV, { entity.origin.x, entity.origin.y , entity.origin.z + 70 });

        if (drawMisc) {

            if (entity.nameId == 4) {

                std::sprintf(str, "Pallet [%dm.]", entity.distTo);

                overlay->draw_text(lowPoint.x, lowPoint.y - 40, str, true, 4, 10);
            }

            else if (entity.nameId == 5) {

                std::sprintf(str, "Gate [%dm.]", entity.distTo);
                overlay->draw_text(highPoint.x, highPoint.y, str, true, 5, 14);
            }



        }
    }

}


void drawESP(Nvidia* overlay) {

    //POINT localOnScreen = WorldToScreen(cameraCache.POV, localOrigin);

    for (Entity player : players2Draw) {

        if (!esp) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }
        POINT screenLeg = WorldToScreen(cameraCache.POV, { player.origin.x, player.origin.y , player.origin.z - 70 });
        POINT screenHead = WorldToScreen(cameraCache.POV, { player.origin.x, player.origin.y , player.origin.z + 85 });

        int deltaY = abs(screenLeg.y - screenHead.y);

        const char* compareStr = "Killer";
        const char* compareStrFem = "FemaleSurvivor";
        const char* compareStrMan = "MaleSurvivor";


        char str[256];

        int colorIndex = 2;



        if (espLine) {
            if (localIsSlasher) {
                colorIndex = 1;
            }
            else if (player.nameId == 3) {
                colorIndex = 1;
            }
            overlay->draw_line(ScreenCenterX, sHeight, screenLeg.x, screenLeg.y, 1.5, colorIndex);

        }

        if (box) {

            if (localIsSlasher) {

                colorIndex = 1;
            }
            else if (player.nameId == 3) {
                colorIndex = 1;
            }

            if (corneredBox) {
                overlay->draw_rect_box(screenHead.x - deltaY / 3.4, screenHead.y, screenHead.x + deltaY / 3.4, screenHead.y, screenLeg.x - deltaY / 3.4, screenLeg.y, screenLeg.x + deltaY / 3.4, screenLeg.y, 1.2, colorIndex);
            }
            else {

                overlay->draw_box(screenHead.x - deltaY / 3.4, screenHead.y, screenHead.x + deltaY / 3.4, screenHead.y, screenLeg.x - deltaY / 3.4, screenLeg.y, screenLeg.x + deltaY / 3.4, screenLeg.y, 3, 10);
                overlay->draw_box(screenHead.x - deltaY / 3.4, screenHead.y, screenHead.x + deltaY / 3.4, screenHead.y, screenLeg.x - deltaY / 3.4, screenLeg.y, screenLeg.x + deltaY / 3.4, screenLeg.y, 1.2, colorIndex);

            }
        }


        if (infoEsp) {

            if (player.nameId == 1) {


                std::sprintf(str, "FemaleSurvivor [%dm.]", player.distTo);

                overlay->draw_text(screenHead.x, screenHead.y - 14, str, true, 0, 9.5);
            }

            else if (player.nameId == 2) {

                std::sprintf(str, "MaleSurvivor [%dm.]", player.distTo);

                overlay->draw_text(screenHead.x, screenHead.y - 14, str, true, 0, 9.5);
            }

            else if (player.nameId == 3) {

                std::sprintf(str, "Killer [%dm.]", player.distTo);

                overlay->draw_text(screenHead.x, screenHead.y - 14, str, true, 0, 9.5);
            }


        }



    }

}

void drawMenu(Nvidia* overlay) {

    overlay->draw_text(15, 5, "SHOW/HIDE [INSERT] vanoware", false, 0, 11);

    if (showmenu && rendering)
    {
        if (esp)
            overlay->draw_text(15, 20, "F1 Esp : ON", false, 2, 11);
        else
            overlay->draw_text(15, 20, "F1 Esp : OFF", false, 1, 11);

        if (infoEsp)
            overlay->draw_text(15, 40, "F2 info Esp : ON", false, 2, 11);
        else
            overlay->draw_text(15, 40, "F2 info Esp : OFF", false, 1, 11);

        if (drawMisc)
            overlay->draw_text(15, 60, "F3 Misc ESP : ON", false, 2, 11);
        else
            overlay->draw_text(15, 60, "F3 Misc ESP : OFF", false, 1, 11);

        if (espLine)
            overlay->draw_text(15, 80, "F4 ESPLine : ON", false, 2, 11);
        else
            overlay->draw_text(15, 80, "F4 ESPLine : OFF", false, 1, 11);


        if (box)
            overlay->draw_text(15, 120, "F6 BOX : ON", false, 2, 11);
        else
            overlay->draw_text(15, 120, "F6 BOX : OFF", false, 1, 11);

        if (corneredBox)
            overlay->draw_text(15, 140, "F7 BOX CORNER MODE : ON", false, 2, 11);
        else
            overlay->draw_text(15, 140, "F7 BOX CORNER MODE : OFF", false, 1, 11);

        if (freecamera)
            overlay->draw_text(15, 160, "F8 freecamera : ON", false, 2, 11);
        else
            overlay->draw_text(15, 160, "F8 freecamera : OFF", false, 1, 11);


    }

}


void updateWindow();


static void _init(Nvidia* overlay)
{
    Sleep(50);
    std::thread(checkKeys).detach();

    std::thread(setupEntities).detach();

    std::thread(updateWindow).detach();

    std::thread(updateCamera).detach();

    std::thread(setCameraCash).detach();

    if (!overlay->window_init())
        return;

    if (!overlay->init_d2d())
        return;

    int i = 0;

    while (true)
    {
        overlay->begin_scene();
        overlay->clear_scene();

        drawESP(overlay);
        drawESPmisc(overlay);
        drawMenu(overlay);

        overlay->end_scene();
    }

    overlay->d2d_shutdown();

    return;
}

void updateWindow() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        hwnd = FindWindowA("UnrealWindow", NULL);
        OverlayWindow = FindWindow("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
        hwnd_active = GetForegroundWindow();

        if (hwnd_active == hwnd) {
            ShowWindow(OverlayWindow, SW_SHOW);
        }
        else {
            ShowWindow(OverlayWindow, SW_HIDE);
        }

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        HDC monitor = GetDC(hDesktop);
        int current = GetDeviceCaps(monitor, VERTRES);
        int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

        sHeight = GetSystemMetrics(SM_CYSCREEN);
        sWidth = GetSystemMetrics(SM_CXSCREEN);

        ScreenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
        ScreenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;
    }
}

int main()
{
	mem::init();

    DWORD procId = mem::find_process("DeadByDaylight-Win64-Shipping.exe");

    std::cout << "waiting for process...";

    process_base = mem::base_address();
    std::cout << "[+] Found DBD Base ---> " << "0x" << std::hex << process_base << std::dec << std::endl;

    mem::fetch_cr3();

    Nvidia* overlay = { 0 };
    _init(overlay);
}
