#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "filling.h"
#include "Ellipse.h"
#include "clipping.h"
// use the following commands to compile and run the code 
// g++ GUI.cpp -o GUI -DUNICODE -D_UNICODE -lgdi32 -lcomdlg32 -municode   
// .\GUI

#define ID_COMBO_SHAPE      101
#define ID_COMBO_COLOR      102
#define ID_COMBO_CURSOR     103
#define ID_BTN_WHITE_BG     104
#define ID_BTN_CLEAR        105
#define ID_BTN_SAVE         106
#define ID_BTN_LOAD         107
#define ID_BTN_DRAW         108
#define ID_STATIC_WELCOME   109
#define ID_COMBO_ALG        110
#define IDC_CUSTOM_COLOR_BUTTON 111
#define IDC_CUSTOM_BCOLOR_BUTTON 112
#define ID_QUARTER_COMBO    113

HFONT hFontLarge, hFontMedium, hFontSmall;
HCURSOR currentCursor = LoadCursorW(NULL, IDC_ARROW);
HBRUSH drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
HWND hDrawingWindow = NULL;
HBITMAP g_hBitmap = NULL;  // Global handle to store the loaded bitmap
bool clear = false;
std::wstring algorithm = L"";
std::wstring shape = L"";
COLORREF color;
COLORREF customColor = RGB(0, 0, 0);
COLORREF bcolor = RGB(0, 0, 0);
std::vector<Point> clickPoints; // Point from filling.h
int clickCount = 0;
int X1, Y1, X2, Y2;
int Xc, Yc, R;
int selectQuarter = 1;

std::vector<POINT> LineMidpoint, LineParametric, LineDirect, CircleDirect, CirclePolar,
CircleIterativePolar, CircleMidpoint, CircleModifiedMidpoint, CircleFillWithLines,
CircleWithCircles, HermitCurve, Square;

std::map<std::wstring, std::vector<std::wstring>> shapeAlgorithmMap = {
    {L"Line", {L"DDA", L"Midpoint", L"Parametric"}},
    {L"Circle", {L"Direct", L"Polar", L"Iterative Polar", L"Midpoint", L"Modified Midpoint"}},
    {L"Circle Fill", {L"Lines", L"Circle"}},
    {L"Square Fill", {L"Hermit Curve [Vertical]"}},
    {L"Rectangle Fill", {L"Bezier Curve [Horizontal]"}},
    {L"Convex Fill", {L"Scanline"}},
    {L"Non-Convex Fill", {L"Polygon Fill"}},
    {L"Flood Fill", {L"Recursive", L"Non-Recursive"}},
    {L"Cardinal Spline Curve", {L"Curve Algorithm"}},
    {L"Ellipse", {L"Direct", L"Polar", L"Midpoint"}},
    {L"Rectangle Clipping", {L"Point", L"Line", L"Polygon"}},
    {L"Square Clipping", {L"Point", L"Line"}},
    {L"Circle Clipping", {L"Point", L"Line"}}
};

std::map<std::wstring, COLORREF> shapeColorMap = {
    {L"Red", RGB(255, 0, 0)},
    {L"Green", RGB(0, 255, 0)},
    {L"Blue", RGB(0, 0, 255)},
    {L"Yellow", RGB(255, 255, 0)},
    {L"Purple", RGB(128, 0, 128)},
};

void drawUI(HWND hwnd) {
    hFontLarge = CreateFontW(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    hFontMedium = CreateFontW(-16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    HWND hRibbon = CreateWindowExW(0, L"STATIC", L"Graphics GUI", WS_CHILD | WS_VISIBLE, 10, 10, 200, 30, hwnd, NULL, NULL, NULL);
    SendMessageW(hRibbon, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hWelcome = CreateWindowExW(0, L"STATIC", L"Welcome to Our Drawing Program!", WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 60, 590, 30, hwnd, (HMENU)ID_STATIC_WELCOME, NULL, NULL);
    SendMessageW(hWelcome, WM_SETFONT, (WPARAM)hFontLarge, TRUE);

    HWND hShapeLabel = CreateWindowExW(0, L"STATIC", L"Shape:", WS_CHILD | WS_VISIBLE, 80, 120, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessageW(hShapeLabel, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeBox = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 120, 250, 200, hwnd, (HMENU)ID_COMBO_SHAPE, NULL, NULL);
    SendMessageW(hShapeBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hAlgorithm = CreateWindowExW(0, L"STATIC", L"Algorithm:", WS_CHILD | WS_VISIBLE, 80, 150, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessageW(hAlgorithm, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hAlgBox = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 150, 250, 200, hwnd, (HMENU)ID_COMBO_ALG, NULL, NULL);
    SendMessageW(hAlgBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeColor = CreateWindowExW(0, L"STATIC", L"Shape Color:", WS_CHILD | WS_VISIBLE, 80, 190, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessageW(hShapeColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeColorBox = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 190, 250, 200, hwnd, (HMENU)ID_COMBO_COLOR, NULL, NULL);
    SendMessageW(hShapeColorBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouse = CreateWindowExW(0, L"STATIC", L"Mouse Shape:", WS_CHILD | WS_VISIBLE, 80, 230, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessageW(hMouse, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouseBox = CreateWindowExW(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 230, 250, 200, hwnd, (HMENU)ID_COMBO_CURSOR, NULL, NULL);
    SendMessageW(hMouseBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hBackground = CreateWindowExW(0, L"BUTTON", L"Change Background Color", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 168, 300, 270, 25, hwnd, (HMENU)ID_BTN_WHITE_BG, NULL, NULL);
    SendMessageW(hBackground, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hCustomColor = CreateWindowW(L"BUTTON", L"Custom Color", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER, 456, 300, 120, 25, hwnd, (HMENU)IDC_CUSTOM_COLOR_BUTTON, NULL, NULL);
    SendMessageW(hCustomColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hCustomBColor = CreateWindowW(L"BUTTON", L"Border Color", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER, 24, 300, 120, 25, hwnd, (HMENU)IDC_CUSTOM_BCOLOR_BUTTON, NULL, NULL);
    SendMessageW(hCustomBColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hClear = CreateWindowExW(0, L"BUTTON", L"Clear Screen", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 24, 350, 120, 25, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
    SendMessageW(hClear, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hDrawButton = CreateWindowExW(0, L"BUTTON", L"Draw", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 168, 350, 120, 25, hwnd, (HMENU)ID_BTN_DRAW, NULL, NULL);
    SendMessageW(hDrawButton, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hSave = CreateWindowExW(0, L"BUTTON", L"Save to File", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 312, 350, 120, 25, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
    SendMessageW(hSave, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hLoad = CreateWindowExW(0, L"BUTTON", L"Load from File", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 456, 350, 120, 25, hwnd, (HMENU)ID_BTN_LOAD, NULL, NULL);
    SendMessageW(hLoad, WM_SETFONT, (WPARAM)hFontMedium, TRUE);
}

void populateShapeAlgorithm(HWND hwnd) {
    HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Line");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Rectangle Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Convex Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Non-Convex Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Flood Fill");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Cardinal Spline Curve");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Ellipse");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Rectangle Clipping");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Clipping");
    SendMessageW(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle Clipping");
    SendMessageW(hShapeBox, CB_SETCURSEL, 0, 0);
    SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_COMBO_SHAPE, CBN_SELCHANGE), (LPARAM)hShapeBox);
}

void populateColorList(HWND hwnd) {
    HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
    for (const auto& pair : shapeColorMap) {
        SendMessageW(hColorBox, CB_ADDSTRING, 0, (LPARAM)pair.first.c_str());
    }
    SendMessageW(hColorBox, CB_SETCURSEL, 0, 0);
}

void populateMouseList(HWND hwnd) {
    HWND hMouseBox = GetDlgItem(hwnd, ID_COMBO_CURSOR);
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Arrow");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"I-Beam");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Cross");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Hand");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Wait");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"No");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size All");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NW-SE");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NE-SW");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size WE");
    SendMessageW(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NS");
    SendMessageW(hMouseBox, CB_SETCURSEL, 0, 0);
}

std::wstring GetComboBoxSelectedText(HWND comboBox) {
    int sel = (int)SendMessageW(comboBox, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) return L"";
    wchar_t buffer[256];
    SendMessageW(comboBox, CB_GETLBTEXT, sel, (LPARAM)buffer);
    return std::wstring(buffer);
}

//******************************************************************************************
// Saving
bool CaptureWindowToBMP(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, width, height);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

    BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    BITMAP bmpScreen;
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = -bmpScreen.bmHeight;  // Negative to flip vertically
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Save File Dialog
    wchar_t filePath[MAX_PATH] = L"";

    // Default file name
    SYSTEMTIME st;
    GetLocalTime(&st);
    swprintf_s(filePath, MAX_PATH, L"drawing_%04d-%02d-%02d_%02d-%02d-%02d.bmp",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"BMP Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"bmp";

    if (!GetSaveFileNameW(&ofn)) {
        MessageBoxW(hwnd, L"Save cancelled.", L"Info", MB_OK | MB_ICONINFORMATION);
        // Cleanup
        SelectObject(hdcMemDC, hOld);
        DeleteObject(hbmScreen);
        DeleteDC(hdcMemDC);
        ReleaseDC(hwnd, hdcWindow);
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);
        return false;
    }

    HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBoxW(hwnd, L"Failed to create file.", L"Error", MB_OK | MB_ICONERROR);
        SelectObject(hdcMemDC, hOld);
        DeleteObject(hbmScreen);
        DeleteDC(hdcMemDC);
        ReleaseDC(hwnd, hdcWindow);
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);
        return false;
    }

    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfType = 0x4D42;  // 'BM'

    DWORD dwBytesWritten;
    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    CloseHandle(hFile);

    // Cleanup
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    SelectObject(hdcMemDC, hOld);
    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);

    std::wstring msg = L"Successfully saved to:\n" + std::wstring(filePath);
    MessageBoxW(hwnd, msg.c_str(), L"Success!", MB_OK | MB_ICONINFORMATION);

    return true;
}


// Loading
void OpenBitmapFile(HWND hwnd) {
    wchar_t filePath[MAX_PATH] = L"";
    OPENFILENAME ofn = { sizeof(ofn) };
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = L"Open Bitmap";

    if (GetOpenFileName(&ofn)) {
        if (g_hBitmap) DeleteObject(g_hBitmap);  // Free old bitmap if any
        g_hBitmap = (HBITMAP)LoadImage(
            NULL,
            filePath,
            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE | LR_CREATEDIBSECTION
        );
        if (g_hBitmap) {
            InvalidateRect(hwnd, NULL, TRUE);  // Force redraw
            std::wstring msg = L"Successfully Loaded " + std::wstring(filePath);
            MessageBoxW(hwnd, msg.c_str(), L"Success!", MB_OK);
        }
        else {
            MessageBox(hwnd, L"Failed to load image!", L"Error", MB_ICONERROR);
        }
    }
}


//******************************************************************************************



// void DrawClippingWindow(HDC hdc) {
//     if (clipWindowDefined) {
//         if (currentWindow == RECTANGLE) {
//             DrawRectangle(hdc, clipRect.X_min, clipRect.Y_min, clipRect.X_max - clipRect.X_min, clipRect.Y_max - clipRect.Y_min, bcolor);
//         }
//         else if (currentWindow == SQUARE) {
//             DrawSquare(hdc, clipRect.X_min, clipRect.Y_min, clipRect.Y_max - clipRect.Y_min, bcolor);
//         }
//         else if (currentWindow == CIRCLE) {
//             DrawCircleMidpoint(hdc, clipCircle.xc, clipCircle.yc, clipCircle.r, bcolor);
//         }
//     }
// }

LRESULT CALLBACK DrawingWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    static PAINTSTRUCT ps;
    static HWND hQuarterCombo;
    static int clickCount = 0;

    switch (msg) {
    case WM_CREATE: {
        hQuarterCombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 10, 10, 100, 100, hwnd, (HMENU)ID_QUARTER_COMBO, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SendMessageW(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"1");
        SendMessageW(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"2");
        SendMessageW(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"3");
        SendMessageW(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"4");
        SendMessageW(hQuarterCombo, CB_SETCURSEL, 0, 0);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_QUARTER_COMBO && HIWORD(wParam) == CBN_SELCHANGE) {
            int index = SendMessageW(hQuarterCombo, CB_GETCURSEL, 0, 0);
            if (index != CB_ERR) {
                selectQuarter = index + 1;
            }
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        hdc = GetDC(hwnd);

        if (shape == L"Rectangle Clipping" || shape == L"Square Clipping" || shape == L"Circle Clipping") {
            if (currentState == IDLE) {
                currentWindow = (shape == L"Rectangle Clipping") ? RECTANGLE : (shape == L"Square Clipping") ? SQUARE : CIRCLE;
                currentState = SET_CLIP_WINDOW_P1;
                clipWindowDefined = false;
                tempPolygonVertices.clear();
                clickPoints.clear();
                clickCount = 0;
                point = Linepoint(x, y);
                currentState = SET_CLIP_WINDOW_P2;
            }
            else if (currentState == SET_CLIP_WINDOW_P2) {
                if (currentWindow == RECTANGLE) {
                    clipRect.X_min = min(point.x, (double)x);
                    clipRect.X_max = max(point.x, (double)x);
                    clipRect.Y_min = min(point.y, (double)y);
                    clipRect.Y_max = max(point.y, (double)y);
                    clipWindowDefined = true;
                }
                else if (currentWindow == SQUARE) {
                    double side = max(abs(x - point.x), abs(y - point.y));
                    clipRect.X_min = point.x;
                    clipRect.X_max = point.x + side;
                    clipRect.Y_min = point.y;
                    clipRect.Y_max = point.y + side;
                    clipWindowDefined = true;
                }
                else if (currentWindow == CIRCLE) {
                    clipCircle.xc = point.x;
                    clipCircle.yc = point.y;
                    clipCircle.r = sqrt(pow(x - point.x, 2) + pow(y - point.y, 2));
                    clipWindowDefined = true;
                }
                currentState = (algorithm == L"Point") ? SET_POINT : (algorithm == L"Line") ? SET_LINE_P1 : SET_POLYGON;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (currentState == SET_POINT && clipWindowDefined) {
                point = Linepoint(x, y);
                if (currentWindow == RECTANGLE || currentWindow == SQUARE) {
                    RectangleClipping(hdc, 0, clipRect, point, point, Poly{});
                }
                else if (currentWindow == CIRCLE) {
                    CircleClipping(hdc, 0, clipCircle, point, point);
                }
                currentState = IDLE;
                clickPoints.clear();
                clickCount = 0;
            }
            else if (currentState == SET_LINE_P1 && clipWindowDefined) {
                lineP1 = Linepoint(x, y);
                currentState = SET_LINE_P2;
            }
            else if (currentState == SET_LINE_P2 && clipWindowDefined) {
                lineP2 = Linepoint(x, y);
                if (currentWindow == RECTANGLE || currentWindow == SQUARE) {
                    RectangleClipping(hdc, 1, clipRect, lineP1, lineP2, Poly{});
                }
                else if (currentWindow == CIRCLE) {
                    CircleClipping(hdc, 1, clipCircle, lineP1, lineP2);
                }
                currentState = IDLE;
                clickPoints.clear();
                clickCount = 0;
            }
            else if (currentState == SET_POLYGON && clipWindowDefined && algorithm == L"Polygon" && shape == L"Rectangle Clipping") {
                tempPolygonVertices.push_back(Linepoint(x, y));
                if (tempPolygonVertices.size() > 1) {
                    DrawLineDDA(hdc, round(tempPolygonVertices[tempPolygonVertices.size() - 2].x),
                        round(tempPolygonVertices[tempPolygonVertices.size() - 2].y),
                        round(tempPolygonVertices.back().x),
                        round(tempPolygonVertices.back().y), RGB(255, 0, 0));
                }
            }
        }
        else {
            X1 = x;
            Y1 = y;
            clickPoints.push_back(Point{ (double)x, (double)y });
            clickCount++;
            Xc = x;
            Yc = y;
        }
        ReleaseDC(hwnd, hdc);
        break;
    }
    case WM_RBUTTONDOWN: {
        if (shape == L"Rectangle Clipping" && algorithm == L"Polygon" && currentState == SET_POLYGON && clipWindowDefined && tempPolygonVertices.size() >= 3) {
            hdc = GetDC(hwnd);
            polygon.vertices = tempPolygonVertices;
            RectangleClipping(hdc, 2, clipRect, Linepoint(0, 0), Linepoint(0, 0), polygon);
            tempPolygonVertices.clear();
            currentState = IDLE;
            clickPoints.clear();
            clickCount = 0;
            ReleaseDC(hwnd, hdc);
        }
        else if (!(shape == L"Rectangle Clipping" || shape == L"Square Clipping" || shape == L"Circle Clipping")) {
            X1 = LOWORD(lParam);
            Y1 = HIWORD(lParam);
            clickPoints.push_back(Point{ (double)X1, (double)Y1 });
            clickCount++;
        }
        break;
    }
    case WM_LBUTTONUP: {
        hdc = GetDC(hwnd);
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        X2 = x;
        Y2 = y;
        int X = x;
        int Y = y;
        R = round((double)(sqrt((double)((x - Xc) * (x - Xc)) + (double)(pow((y - Yc), 2)))));

        if (shape == L"Line") {
            if (algorithm == L"DDA") DrawLineDDA(hdc, X1, Y1, X2, Y2, color);
            else if (algorithm == L"Midpoint") DrawLineMidpoint(hdc, X1, Y1, X2, Y2, color);
            else if (algorithm == L"Parametric") DrawLineParametric(hdc, X1, Y1, X2, Y2, color);
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Circle") {
            if (algorithm == L"Direct") DrawCircleDirect(hdc, Xc, Yc, R, color);
            else if (algorithm == L"Polar") DrawCirclePolar(hdc, Xc, Yc, R, color);
            else if (algorithm == L"Iterative Polar") DrawCircleIterativePolar(hdc, Xc, Yc, R, color);
            else if (algorithm == L"Midpoint") DrawCircleMidpoint(hdc, Xc, Yc, R, color);
            else if (algorithm == L"Modified Midpoint") DrawCircleModifiedMidpoint(hdc, Xc, Yc, R, color);
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Circle Fill") {
            int quarter = selectQuarter;
            if (algorithm == L"Lines") {
                DrawCircleModifiedMidpoint(hdc, Xc, Yc, R, RGB(0, 0, 0));
                FillingCircleWithLines(hdc, Xc, Yc, R, quarter, 3, color);
            }
            else if (algorithm == L"Circle") {
                DrawCircleModifiedMidpoint(hdc, Xc, Yc, R, RGB(0, 0, 0));
                FillingCircleWithCircles(hdc, Xc, Yc, R, quarter, 3, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Square Fill") {
            if (algorithm == L"Hermit Curve [Vertical]") {
                int dx = X2 - X1;
                int dy = Y2 - Y1;
                int length = max(abs(dx), abs(dy));
                if (dx < 0) X2 = X1 - length; else X2 = X1 + length;
                if (dy < 0) Y2 = Y1 - length; else Y2 = Y1 + length;
                DrawSquare(hdc, X1, Y1, length, bcolor);
                FillingSquareWithHermiteCurves(hdc, X1, Y1, length, 5, bcolor, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Rectangle Fill") {
            if (algorithm == L"Bezier Curve [Horizontal]") {
                int x_min = min(X1, X2);
                int y_min = min(Y1, Y2);
                int length = abs(X2 - X1);
                int width = abs(Y2 - Y1);
                DrawRectangle(hdc, x_min, y_min, length, width, bcolor);
                FillingRectangleWithBezierCurves(hdc, x_min, y_min, length, width, 5, bcolor, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Convex Fill") {
            if (algorithm == L"Scanline") {
                std::vector<Point> testConvex = { {100, 100}, {200, 100}, {150, 200}, {170, 250}, {90, 250} };
                CurveFill(hdc, testConvex, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Non-Convex Fill") {
            if (algorithm == L"Polygon Fill") {
                std::vector<Point> testPolygon = { {100, 100}, {200, 100}, {150, 200}, {170, 250}, {90, 250} };
                FillPolygon(hdc, color, testPolygon);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Flood Fill") {
            int x_min = min(X1, X2);
            int y_min = min(Y1, Y2);
            int length = abs(X2 - X1);
            int width = abs(Y2 - Y1);
            DrawRectangle(hdc, x_min, y_min, length, width, bcolor);
            int seedX = x_min + length / 2;
            int seedY = y_min + width / 2;
            COLORREF bc = GetPixel(hdc, seedX, seedY);
            if (algorithm == L"Recursive") FloodFillRecursive(hdc, seedX, seedY, bc, bcolor, color);
            else if (algorithm == L"Non-Recursive") FloodFillNonRecursive(hdc, seedX, seedY, bc, bcolor, color);
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Cardinal Spline Curve") {
            if (algorithm == L"Curve Algorithm") {
                CardinalSplineCurve(hdc, clickPoints, 0.5, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Ellipse") {
            int dx = X2 - X1;
            int dy = Y2 - Y1;
            int a = abs(dx);
            int b = abs(dy);
            if (algorithm == L"Direct") DirectEllipse(hdc, a, b, Xc, Yc, color);
            else if (algorithm == L"Polar") PolarEllipse(hdc, a, b, Xc, Yc, color);
            else if (algorithm == L"Midpoint") MidPointEllipse(hdc, a, b, Xc, Yc, color);
            clickPoints.clear();
            clickCount = 0;
        }
        ReleaseDC(hwnd, hdc);
        break;
    }
    case WM_PAINT: {
        hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, drawingBackgroundBrush);

        if (g_hBitmap && !clear) {
            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, g_hBitmap);

            BITMAP bmp;
            GetObject(g_hBitmap, sizeof(bmp), &bmp);

            BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

            DeleteDC(hMemDC);
        }

        if (clear) {
            clear = !clear;
        }

        if (shape == L"Rectangle Clipping" || shape == L"Square Clipping" || shape == L"Circle Clipping") {
            DrawClippingWindow(hdc);
        }
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SETCURSOR: {
        SetCursor(currentCursor);
        return TRUE;
    }
    case WM_DESTROY: {
        if (hQuarterCombo) DestroyWindow(hQuarterCombo);
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND CreateDrawingWindow(HWND parent) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = DrawingWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"DrawingWindowClass";
    RegisterClassW(&wc);

    return CreateWindowExW(0, L"DrawingWindowClass", L"Drawing Canvas", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 700, 150, 600, 500, parent, NULL, GetModuleHandleW(NULL), NULL);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const int titleBarHeight = 40;
    static RECT closeButtonRect = { 560, 5, 590, 35 };

    switch (msg) {
    case WM_ERASEBKGND: {
        return 1;
    }
    case WM_CREATE: {
        drawUI(hwnd);
        populateShapeAlgorithm(hwnd);
        populateColorList(hwnd);
        populateMouseList(hwnd);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        FillRect(hdc, &clientRect, drawingBackgroundBrush);
        HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
        RECT ribbonRect = { 0, 0, 600, titleBarHeight };
        FillRect(hdc, &ribbonRect, hBlack);
        DeleteObject(hBlack);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        TextOutW(hdc, 10, 10, L"Graphics GUI", lstrlenW(L"Graphics GUI"));
        DrawTextW(hdc, L"X", 1, &closeButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, 0, 0, 600, 400);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(hPen);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hStatic = (HWND)lParam;
        int id = GetDlgCtrlID(hStatic);
        wchar_t text[256];
        GetWindowTextW(hStatic, text, 256);
        if (wcscmp(text, L"Graphics GUI") == 0) {
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkColor(hdcStatic, RGB(0, 0, 0));
            static HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            return (INT_PTR)hBrush;
        }
        if (id == ID_STATIC_WELCOME) {
            SetTextColor(hdcStatic, RGB(0, 0, 255));
            SetBkColor(hdcStatic, RGB(255, 255, 255));
            static HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            return (INT_PTR)whiteBrush;
        }
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (y < titleBarHeight) {
            if (x >= closeButtonRect.left && x <= closeButtonRect.right && y >= closeButtonRect.top && y <= closeButtonRect.bottom) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
            else {
                SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
        break;
    }
    case WM_COMMAND: {
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_SHAPE) {
            HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
            HWND hAlgBox = GetDlgItem(hwnd, ID_COMBO_ALG);
            int index = SendMessageW(hShapeBox, CB_GETCURSEL, 0, 0);
            wchar_t selectedShape[100];
            SendMessageW(hShapeBox, CB_GETLBTEXT, index, (LPARAM)selectedShape);
            SendMessageW(hAlgBox, CB_RESETCONTENT, 0, 0);
            for (const auto& alg : shapeAlgorithmMap[selectedShape]) {
                SendMessageW(hAlgBox, CB_ADDSTRING, 0, (LPARAM)alg.c_str());
            }
            SendMessageW(hAlgBox, CB_SETCURSEL, 0, 0);
        }
        if (LOWORD(wParam) == IDC_CUSTOM_COLOR_BUTTON) {
            CHOOSECOLORW cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = customColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColorW(&cc)) {
                customColor = cc.rgbResult;
                shapeColorMap[L"Custom"] = customColor;
                HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
                int index = (int)SendMessageW(hColorBox, CB_FINDSTRINGEXACT, -1, (LPARAM)L"Custom");
                if (index == CB_ERR) index = (int)SendMessageW(hColorBox, CB_ADDSTRING, 0, (LPARAM)L"Custom");
                SendMessageW(hColorBox, CB_SETCURSEL, index, 0);
            }
        }
        if (LOWORD(wParam) == ID_BTN_DRAW) {
            HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
            HWND hAlgBox = GetDlgItem(hwnd, ID_COMBO_ALG);
            HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
            shape = GetComboBoxSelectedText(hShapeBox);
            algorithm = GetComboBoxSelectedText(hAlgBox);
            std::wstring colorName = GetComboBoxSelectedText(hColorBox);
            color = shapeColorMap.count(colorName) ? shapeColorMap[colorName] : customColor;
            if (!hDrawingWindow || !IsWindow(hDrawingWindow)) {
                hDrawingWindow = CreateDrawingWindow(hwnd);
            }
            MessageBoxW(hwnd, (L"Drawing: " + shape + L" using " + algorithm + L" in color " + colorName).c_str(), L"Draw", MB_OK);
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_CURSOR) {
            HWND hMouseBox = GetDlgItem(hwnd, ID_COMBO_CURSOR);
            std::wstring cursorName = GetComboBoxSelectedText(hMouseBox);
            if (cursorName == L"Arrow") currentCursor = LoadCursorW(NULL, IDC_ARROW);
            else if (cursorName == L"I-Beam") currentCursor = LoadCursorW(NULL, IDC_IBEAM);
            else if (cursorName == L"Cross") currentCursor = LoadCursorW(NULL, IDC_CROSS);
            else if (cursorName == L"Hand") currentCursor = LoadCursorW(NULL, IDC_HAND);
            else if (cursorName == L"Wait") currentCursor = LoadCursorW(NULL, IDC_WAIT);
            else if (cursorName == L"No") currentCursor = LoadCursorW(NULL, IDC_NO);
            else if (cursorName == L"Size All") currentCursor = LoadCursorW(NULL, IDC_SIZEALL);
            else if (cursorName == L"Size NW-SE") currentCursor = LoadCursorW(NULL, IDC_SIZENWSE);
            else if (cursorName == L"Size NE-SW") currentCursor = LoadCursorW(NULL, IDC_SIZENESW);
            else if (cursorName == L"Size WE") currentCursor = LoadCursorW(NULL, IDC_SIZEWE);
            else if (cursorName == L"Size NS") currentCursor = LoadCursorW(NULL, IDC_SIZENS);
            else currentCursor = LoadCursorW(NULL, IDC_ARROW);

            SetCursor(currentCursor);
            InvalidateRect(hwnd, NULL, FALSE);
            if (hDrawingWindow && IsWindow(hDrawingWindow)) InvalidateRect(hDrawingWindow, NULL, FALSE);
        }
        if (LOWORD(wParam) == ID_BTN_CLEAR) {
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                clickPoints.clear();
                clickCount = 0;
                currentWindow = NONE;
                currentObject = OBJ_NONE;
                currentState = IDLE;
                clipWindowDefined = false;
                tempPolygonVertices.clear();
                InvalidateRect(hDrawingWindow, NULL, TRUE);
                clear = true;
            }
        }
        if (LOWORD(wParam) == ID_BTN_WHITE_BG) {
            CHOOSECOLORW cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = customColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColorW(&cc)) {
                drawingBackgroundBrush = CreateSolidBrush(cc.rgbResult);
            }
            else {
                drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
            }
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                InvalidateRect(hDrawingWindow, NULL, TRUE);
            }
        }
        if (LOWORD(wParam) == IDC_CUSTOM_BCOLOR_BUTTON) {
            CHOOSECOLORW cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = bcolor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColorW(&cc)) {
                bcolor = cc.rgbResult;
            }
        }

        // Save btn
        if (LOWORD(wParam) == ID_BTN_SAVE) {
            CaptureWindowToBMP(hDrawingWindow);
        }

        // Load btn
        if (LOWORD(wParam) == ID_BTN_LOAD) {
            OpenBitmapFile(hDrawingWindow);
        }

        break;
    }
    case WM_SETCURSOR: {
        SetCursor(currentCursor);
        return TRUE;
    }
    case WM_DESTROY: {
        DeleteObject(hFontLarge);
        DeleteObject(hFontMedium);
        DeleteObject(hFontSmall);
        DeleteObject(drawingBackgroundBrush);
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MainMenuWindow";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, NULL, WS_POPUPWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}