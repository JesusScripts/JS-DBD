#include "includes.hpp"
#include "driver.hpp"
#include <iostream>
#include <thread>
#include "crypt.h"

void main()
{
    if (!mem::init())
    {
        printf(_XOR_("Driver initialization failed :(\n"));
        Sleep(3000);
        exit(0);
    }
    if (!mem::is_loaded())
    {
        printf(_XOR_("Driver is not loaded :(\n"));
        Sleep(3000);
        exit(0);
    }

    printf(_XOR_("Waiting for DeadByDaylight..."));

    while (game_wnd == 0)
    {
        Sleep(1);
        processID = mem::find_process(L"DeadByDaylight-Win64-Shipping.exe");
        game_wnd = get_process_wnd(processID);
        Sleep(1);
    }

    system("cls");
    printf(_XOR_("Jo"));

    if (!input::init())
    {
        printf(_XOR_("The input was not initialized :(\n"));
        Sleep(3000);
        exit(0);
    }

    if (!gui::init())
    {
        printf(_XOR_("The gui was not initialized :(\n"));
        Sleep(3000);
        exit(0);
    }

    virtualaddy = mem::base_address();
    printf(_XOR_("Base address: %p\n"), virtualaddy);
    cr3 = mem::fetch_cr3();
//    printf(_XOR_("CR3: %p\n"), cr3);

    if (!virtualaddy)
    {
        printf(_XOR_("The driver couldn't get the base address\n"));
        Sleep(3000);
        exit(0);
    }


    create_overlay();
    directx_init();
    render_loop();
}
