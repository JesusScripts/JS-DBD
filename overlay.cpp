#include "overlay.h"
#include <comdef.h>
#include <corecrt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#pragma warning(disable : 4996)

static HWND win;

/*
Window functions
*/

auto Nvidia::window_set_style() -> void {
    int i = 0;

    i = (int)GetWindowLong(win, -20);

    SetWindowLongPtr(win, -20, (LONG_PTR)(i | 0x20));
}

auto Nvidia::window_set_transparency() -> void {
    MARGINS margin;
    UINT opacity_flag, color_key_flag, color_key = 0;
    UINT opacity = 0;

    margin.cyBottomHeight = -1;
    margin.cxLeftWidth = -1;
    margin.cxRightWidth = -1;
    margin.cyTopHeight = -1;

    DwmExtendFrameIntoClientArea(win, &margin);

    opacity_flag = 0x02;
    color_key_flag = 0x01;
    color_key = 0x000000;
    opacity = 0xFF;

   SetLayeredWindowAttributes(win, color_key, opacity, opacity_flag);
}

auto Nvidia::window_set_top_most() -> void {
    SetWindowPos(win, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);
}

auto Nvidia::retrieve_window() -> HWND { return win; }

// Hijacking method down here.

auto Nvidia::window_init() -> BOOL {
    win = FindWindow("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
    if (!win)
        return FALSE;

    Nvidia::window_set_style();
    Nvidia::window_set_transparency();
    Nvidia::window_set_top_most();

    ShowWindow(win, SW_SHOW);

    return TRUE;
}
/*
Overlay functions
*/

ID2D1Factory* d2d_factory;
ID2D1HwndRenderTarget* tar;

IDWriteFactory* write_factory;
IDWriteFactory* smaller_write_factory;

IDWriteTextLayout* textLayout;
DWRITE_TEXT_METRICS formatMetrics;

ID2D1SolidColorBrush* brush;
ID2D1SolidColorBrush* blue_brush;
ID2D1SolidColorBrush* purple_brush;
ID2D1SolidColorBrush* red_brush;
ID2D1SolidColorBrush* green_brush;
ID2D1SolidColorBrush* yellow_brush;
ID2D1SolidColorBrush* black_brush;

ID2D1SolidColorBrush* cur_brush;

IDWriteTextFormat* format;
IDWriteTextFormat* smaller_format;

auto Nvidia::d2d_shutdown() -> void {
    // Release
    tar->Release();
    write_factory->Release();
    brush->Release();
    red_brush->Release();
    blue_brush->Release();
    green_brush->Release();
    yellow_brush->Release();
    purple_brush->Release();
    d2d_factory->Release();
}

auto Nvidia::init_d2d() -> BOOL {
    HRESULT ret;
    RECT rc;

    // Initialize D2D here
    ret = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory);
    if (FAILED(ret))
        return FALSE;

    ret =
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            (IUnknown**)(&write_factory));
    if (FAILED(ret))
        return FALSE;

    ret =
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            (IUnknown**)(&smaller_write_factory));
    if (FAILED(ret))
        return FALSE;


    GetClientRect(Nvidia::retrieve_window(), &rc);

    ret = d2d_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
                D2D1_ALPHA_MODE_PREMULTIPLIED)),
        D2D1::HwndRenderTargetProperties(
            Nvidia::retrieve_window(),
            D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
        &tar);
    if (FAILED(ret))
        return FALSE;

    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &red_brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &green_brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &blue_brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &yellow_brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Purple), &purple_brush);
    tar->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &black_brush);

    return TRUE;
}

auto Nvidia::begin_scene() -> void { tar->BeginDraw(); }

auto Nvidia::end_scene() -> void { tar->EndDraw(); }

auto Nvidia::clear_scene() -> void { tar->Clear(); }

wchar_t* ConvertCharToWchar(const char* charString)
{
    int charLength = strlen(charString) + 1;
    int wcharLength = MultiByteToWideChar(CP_UTF8, 0, charString, charLength, NULL, 0);

    wchar_t* wcharString = new wchar_t[wcharLength];

    MultiByteToWideChar(CP_UTF8, 0, charString, charLength, wcharString, wcharLength);

    return wcharString;
}

wchar_t* charToWChar(const char* input) {
    int length = strlen(input) + 1;
    wchar_t* output = new wchar_t[length];

    mbstowcs(output, input, length);

    return output;
}

auto Nvidia::draw_text(int x, int y, const char* str, bool center, int color, float font_size, ...) -> void {
    char buf[4096];
    int len = 0;
    wchar_t* b = charToWChar(str);

    write_factory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, font_size, L"en-us", &format);
    write_factory->CreateTextLayout(b, wcslen(b), format, 1920, 1080, &textLayout);

    textLayout->GetMetrics(&formatMetrics);
    float textWidth = formatMetrics.widthIncludingTrailingWhitespace;


    if (color == 0)
        cur_brush = brush;
    else if (color == 1)
        cur_brush = red_brush;
    else if (color == 2)
        cur_brush = green_brush;
    else if (color == 3)
        cur_brush = blue_brush;
    else if (color == 4)
        cur_brush = yellow_brush;
    else if (color == 5)
        cur_brush = purple_brush;
    else if (color == 10)
        cur_brush = black_brush;

    if (center) {
        x -= (int)textWidth / 2;
    }

    tar->DrawTextLayout(D2D1::Point2F(x, y), textLayout, cur_brush);
    // tar->DrawText(b, len, format, D2D1::RectF(x, y, 1920, 1080), cur_brush,D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
}

auto Nvidia::draw_line(int x, int y, int x2, int y2, float thickness, int color, ...)-> void {


    if (color == 0)
        cur_brush = brush;
    else if (color == 1)
        cur_brush = red_brush;
    else if (color == 2)
        cur_brush = green_brush;
    else if (color == 3)
        cur_brush = blue_brush;
    else if (color == 4)
        cur_brush = yellow_brush;
    else if (color == 5)
        cur_brush = purple_brush;
    else if (color == 10)
        cur_brush = black_brush;


    tar->DrawLine(D2D1::Point2F(x, y), D2D1::Point2F(x2, y2), cur_brush, thickness);
}

auto Nvidia::draw_box(int x, int y, int x2, int y2, int x3, int y3, int x4, int y4, float thickness, int color, ...) -> void {

    draw_line(x, y, x2, y2, thickness, color);

    draw_line(x3, y3, x4, y4, thickness, color);

    draw_line(x, y, x3, y3, thickness, color);

    draw_line(x2, y2, x4, y4, thickness, color);

}

auto Nvidia::draw_rect_box(int x, int y, int x2, int y2, int x3, int y3, int x4, int y4, float thickness, int color, ...) -> void {

    int mid_x12 = (x + x2) / 2;
    int mid_y12 = (y + y2) / 2;

    int mid_x23 = (x3 + x4) / 2;
    int mid_y23 = (y3 + y4) / 2;

    int mid_x13 = (x + x3) / 2;
    int mid_y13 = (y + y3) / 2;

    int mid_x24 = (x2 + x4) / 2;
    int mid_y24 = (y2 + y4) / 2;

    int gap1 = sqrt(pow(y3 - y, 2) + pow(x3 - x, 2)) / 6;
    int gap2 = sqrt(pow(y4 - y2, 2) + pow(x4 - x2, 2)) / 6;

    int gap3 = (x2 - x) / 6;
    int gap4 = (x4 - x3) / 6;

    draw_line(x, y, mid_x12 - gap3, mid_y12, thickness, color);
    draw_line(mid_x12 + gap3, mid_y12, x2, y2, thickness, color);

    draw_line(x3, y3, mid_x23 - gap4, mid_y23, thickness, color);
    draw_line(mid_x23 + gap4, mid_y23, x4, y4, thickness, color);

    draw_line(x, y, mid_x13, mid_y13 - gap1, thickness, color);
    draw_line(mid_x13, mid_y13 + gap1, x3, y3, thickness, color);

    draw_line(x2, y2, mid_x24, mid_y24 - gap2, thickness, color);
    draw_line(mid_x24, mid_y24 + gap2, x4, y4, thickness, color);


}


auto Nvidia::clear_screen() -> void {
    Nvidia::begin_scene();
    Nvidia::clear_scene();
    Nvidia::end_scene();
}
