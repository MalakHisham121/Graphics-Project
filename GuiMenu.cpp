#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>


#define ID_COMBO_SHAPE      101
#define ID_COMBO_COLOR      102
#define ID_COMBO_CURSOR     103
#define ID_BTN_WHITE_BG     104
#define ID_BTN_CLEAR        105
#define ID_BTN_SAVE         106
#define ID_BTN_LOAD         107
#define ID_BTN_DRAW         108
#define ID_STATIC_WELCOME   1000
#define ID_COMBO_ALG        109
#define IDC_CUSTOM_COLOR_BUTTON 110

HFONT hFontLarge, hFontMedium, hFontSmall;
COLORREF customColor = RGB(0, 0, 0);  // Default black
HCURSOR currentCursor = LoadCursor(NULL, IDC_ARROW); // Default
HBRUSH drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // Default: Black
HWND hDrawingWindow = NULL;

COLORREF color;
POINT clickPoints[2];
int clickCount = 0;




std::map<std::wstring, std::vector<std::wstring>> shapeAlgorithmMap = {
    {L"Line", {L"DDA", L"Midpoint", L"Parametric"}},
    {L"Circle", {L"Direct", L"Polar", L"Iterative Polar", L"Midpoint", L"Modified Midpoint"}},
    {L"Circle Fill", {L"Lines", L"Circle"}},
    {L"Square Fill", {L"Hermit Curve [Vertical]"}},
    {L"Rectangle Fill", {L"Bezier  Curve [Horizontal]"}},
    {L"Convex Fill", {L"Scanline"}},    //
    {L"Non-Convex Fill", {L"Polygon Fill"}},    //
    {L"Flood Fill", {L"Recursive", L"Non-Recursive"}},
    {L"Cardinal Spline Curve", {L"Curve Algorithm"}},   //
    {L"Ellipse", {L"Direct", L"Polar", L"Midpoint",}},
    {L"Rectangle Clipping", {L"Point", L"Line", L"Polygon"}},
    {L"Square Clipping", {L"Point", L"Line"}}
};

std::map<std::wstring, COLORREF> shapeColorMap = {
    {L"Red", RGB(255, 0, 0)},
    {L"Green", RGB(0, 255, 0)},
    {L"Blue", RGB(0, 0, 255)},
    {L"Yellow", RGB(255, 255, 0)},
    {L"Purple", RGB(128, 0, 128)},
};


void drawUI(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Large font (Welcome text)
    hFontLarge = CreateFont(
        -18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    // Medium font (Labels)
    hFontMedium = CreateFont(
        -16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );


    // UI Elements (below custom ribbon)
    HWND hRibbon = CreateWindowEx(0, L"STATIC", L"Graphics GUI",
        WS_CHILD | WS_VISIBLE,
        10, 10, 200, 30,
        hwnd, NULL, NULL, NULL);
    SendMessage(hRibbon, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hWelcome = CreateWindowEx(0, L"STATIC", L"Welcome to Our Drawing Program!",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        5, 60, 590, 30,
        hwnd, (HMENU)ID_STATIC_WELCOME, NULL, NULL);
    SendMessage(hWelcome, WM_SETFONT, (WPARAM)hFontLarge, TRUE);

    HWND hShapeLabel = CreateWindowEx(0, L"STATIC", L"Shape:",
        WS_CHILD | WS_VISIBLE,
        50 + 30, 130 - 10, 120, 25,
        hwnd, NULL, NULL, NULL);
    SendMessage(hShapeLabel, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeBox = CreateWindowEx(0, L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        270, 130 - 10, 250, 200,
        hwnd, (HMENU)ID_COMBO_SHAPE, NULL, NULL);
    SendMessage(hShapeBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    //------------------------------------------------------------------------------
    HWND hAlgorithm = CreateWindowEx(0, L"STATIC", L"Algorithm:",
        WS_CHILD | WS_VISIBLE,
        50 + 30, 130 + 30, 120, 25,
        hwnd, NULL, NULL, NULL);
    SendMessage(hAlgorithm, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hAlgBox = CreateWindowEx(0, L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        270, 130 + 30, 250, 200,
        hwnd, (HMENU)ID_COMBO_ALG, NULL, NULL);
    SendMessage(hAlgBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);
    //------------------------------------------------------------------------------

    HWND hShapeColor = CreateWindowEx(0, L"STATIC", L"Shape Color:",
        WS_CHILD | WS_VISIBLE,
        50 + 30, 170 + 30, 120, 25,
        hwnd, NULL, NULL, NULL);
    SendMessage(hShapeColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeColorBox = CreateWindowEx(0, L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        270, 170 + 30, 250, 200,
        hwnd, (HMENU)ID_COMBO_COLOR, NULL, NULL);
    SendMessage(hShapeColorBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouse = CreateWindowEx(0, L"STATIC", L"Mouse Shape:",
        WS_CHILD | WS_VISIBLE,
        50 + 30, 210 + 30, 120, 25,
        hwnd, NULL, NULL, NULL);
    SendMessage(hMouse, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouseBox = CreateWindowEx(0, L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        270, 210 + 30, 250, 200,
        hwnd, (HMENU)ID_COMBO_CURSOR, NULL, NULL);
    SendMessage(hMouseBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    //-----------------------------------------------------------------------------
    HWND hBackground = CreateWindowEx(0, L"BUTTON", L"Change Background to White",
        WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
        80, 300, 240, 25,
        hwnd, (HMENU)ID_BTN_WHITE_BG, NULL, NULL);      // 180, 300, 240, 25
    SendMessage(hBackground, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hCustomColor = CreateWindow(L"BUTTON", L"Custom Color",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER,
        400, 300, 120, 25,
        hwnd, (HMENU)IDC_CUSTOM_COLOR_BUTTON, NULL, NULL);
    SendMessage(hCustomColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);
    //-----------------------------------------------------------------------------

    HWND hClear = CreateWindowEx(0, L"BUTTON", L"Clear Screen",
        WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
        24, 350, 120, 25,
        hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
    SendMessage(hClear, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hDrawButton = CreateWindowEx(0, L"BUTTON", L"Draw",
        WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
        168, 350, 120, 25,
        hwnd, (HMENU)ID_BTN_DRAW, NULL, NULL);
    SendMessage(hDrawButton, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hSave = CreateWindowEx(0, L"BUTTON", L"Save to File",
        WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
        312, 350, 120, 25,
        hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
    SendMessage(hSave, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hLoad = CreateWindowEx(0, L"BUTTON", L"Load from File",
        WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
        456, 350, 120, 25,
        hwnd, (HMENU)ID_BTN_LOAD, NULL, NULL);
    SendMessage(hLoad, WM_SETFONT, (WPARAM)hFontMedium, TRUE);
    return;
}

void populateShapeAlgorithm(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Line");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Convex Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Non-Convex Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Flood Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Cardinal Spline Curve");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Ellipse");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Rectangle Clipping");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Clipping");

    SendMessage(hShapeBox, CB_SETCURSEL, 0, 0);  // Select "Line"
    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_COMBO_SHAPE, CBN_SELCHANGE), (LPARAM)hShapeBox);
};

void populateColorList(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
    for (const auto& pair : shapeColorMap) {
        SendMessage(hColorBox, CB_ADDSTRING, 0, (LPARAM)pair.first.c_str());
    }
};

void populateMouseList(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND hMouseBox = GetDlgItem(hwnd, ID_COMBO_CURSOR);

    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Arrow");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"I-Beam");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Cross");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Hand");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Wait");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"No");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size All");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NW-SE");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NE-SW");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size WE");
    SendMessage(hMouseBox, CB_ADDSTRING, 0, (LPARAM)L"Size NS");

    SendMessage(hMouseBox, CB_SETCURSEL, 0, 0);

};

std::wstring GetComboBoxSelectedText(HWND comboBox) {
    int sel = (int)SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) return L"";
    wchar_t buffer[256];
    SendMessage(comboBox, CB_GETLBTEXT, sel, (LPARAM)buffer);
    return std::wstring(buffer);
}


//-----------------------------------------------------------------------------------------------
void drawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = max(abs(dx), abs(dy));
    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        SetPixel(hdc, round(x), round(y), color);
        x += xInc;
        y += yInc;
    }
}

LRESULT CALLBACK DrawingWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static POINT clickPoints[2];
    static int clickCount = 0;
    COLORREF drawColor = color;  // Red by default

    switch (msg) {
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        clickPoints[clickCount] = { x, y };
        clickCount++;

        if (clickCount == 2) {
            HDC hdc = GetDC(hwnd);
            drawLineDDA(hdc,
                clickPoints[0].x, clickPoints[0].y,
                clickPoints[1].x, clickPoints[1].y,
                drawColor);
            ReleaseDC(hwnd, hdc);
            clickCount = 0;
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, drawingBackgroundBrush);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SETCURSOR: {
        SetCursor(currentCursor);
        return TRUE;
    }
    case WM_COMMAND: {
        
        // Clear btn
        if (LOWORD(wParam) == ID_BTN_CLEAR) {
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                InvalidateRect(hDrawingWindow, NULL, TRUE); // Repaints, triggering WM_PAINT
            }
            break;
        }
        // Bkg white btn
        if (LOWORD(wParam) == ID_BTN_WHITE_BG) {
            drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // White background
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                InvalidateRect(hDrawingWindow, NULL, TRUE); // Refresh the window
            }
            //InvalidateRect(hDrawingWindow, NULL, TRUE);
            break;
        }

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


HWND CreateDrawingWindow(HWND parent) {
    WNDCLASS wc = {};
    //wc.lpfnWndProc = DefWindowProc;
    wc.lpfnWndProc = DrawingWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"DrawingWindowClass";
    RegisterClass(&wc);
    SetWindowLongPtr(hDrawingWindow, GWLP_USERDATA, (LONG_PTR)color);


    return CreateWindowEx(
        0, L"DrawingWindowClass", L"Drawing Canvas",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        700, 100, 600, 500,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
}

//-----------------------------------------------------------------------------------------------



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const int titleBarHeight = 40;
    static RECT closeButtonRect = { 560, 5, 590, 35 }; // Custom close button

    switch (msg) {
    case WM_ERASEBKGND: {
        // Prevent default erase (avoid flicker)
        return 1;
    }
    case WM_CREATE: {

        drawUI(hwnd, msg, wParam, lParam);
        populateShapeAlgorithm(hwnd, msg, wParam, lParam);
        populateColorList(hwnd, msg, wParam, lParam);
        populateMouseList(hwnd, msg, wParam, lParam);

        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Fill background with current brush color
        FillRect(hdc, &clientRect, drawingBackgroundBrush);

        // Draw custom black ribbon
        HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
        RECT ribbonRect = { 0, 0, 600, titleBarHeight };
        FillRect(hdc, &ribbonRect, hBlack);
        DeleteObject(hBlack);

        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        TextOut(hdc, 10, 10, L"Graphics GUI", lstrlen(L"Graphics GUI"));

        // Draw custom close (X) button
        DrawText(hdc, L"X", 1, &closeButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Draw custom black border
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(hdc, 0, 0, 600, 400); // match your window size

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(hPen);

        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hStatic = (HWND)lParam;

        int id = GetDlgCtrlID(hStatic);
        wchar_t text[100];
        GetWindowText(hStatic, text, 100);

        // Custom ribbon background for label
        if (wcscmp(text, L"Graphics GUI") == 0) {
            SetTextColor(hdcStatic, RGB(255, 255, 255));
            SetBkColor(hdcStatic, RGB(0, 0, 0));
            static HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            return (INT_PTR)hBrush;
        }

        if (id == ID_STATIC_WELCOME) {
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, RGB(255, 255, 255));
            static HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            return (INT_PTR)whiteBrush;
        }

        //return DefWindowProc(hwnd, WM_CTLCOLORSTATIC, wParam, lParam);
        return (INT_PTR)GetStockObject(WHITE_BRUSH); // for fallback

    }

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Handle dragging or closing from ribbon
        if (y < titleBarHeight) {
            if (x >= closeButtonRect.left && x <= closeButtonRect.right &&
                y >= closeButtonRect.top && y <= closeButtonRect.bottom) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            else {
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
        break;
    }

    case WM_COMMAND:{
        // Algorithms drop-down
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_SHAPE) {
            HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
            HWND hAlgBox = GetDlgItem(hwnd, ID_COMBO_ALG);

            int index = SendMessage(hShapeBox, CB_GETCURSEL, 0, 0);
            wchar_t selectedShape[100];
            SendMessage(hShapeBox, CB_GETLBTEXT, index, (LPARAM)selectedShape);

            // Clear previous algorithm items
            SendMessage(hAlgBox, CB_RESETCONTENT, 0, 0);

            // Add new algorithm options
            for (const auto& alg : shapeAlgorithmMap[selectedShape]) {
                SendMessage(hAlgBox, CB_ADDSTRING, 0, (LPARAM)alg.c_str());
            }

            // Optional: Select first item
            SendMessage(hAlgBox, CB_SETCURSEL, 0, 0);
        }

        // Colors drop-down
        if (LOWORD(wParam) == IDC_CUSTOM_COLOR_BUTTON)
        {
            CHOOSECOLOR cc;
            static COLORREF acrCustClr[16]; // Array of custom colors
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = customColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc)) {
                customColor = cc.rgbResult;

                // Optional: Add the selected custom color to your color map for drawing
                shapeColorMap[L"Custom"] = customColor;

                // Optional: Set "Custom" in the color combo box
                HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
                int index = (int)SendMessage(hColorBox, CB_FINDSTRINGEXACT, -1, (LPARAM)L"Custom");
                if (index == CB_ERR) {
                    index = (int)SendMessage(hColorBox, CB_ADDSTRING, 0, (LPARAM)L"Custom");
                }
                SendMessage(hColorBox, CB_SETCURSEL, index, 0);
            }
        }

        // Draw btn
        if (LOWORD(wParam) == ID_BTN_DRAW) {
            HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
            HWND hAlgBox = GetDlgItem(hwnd, ID_COMBO_ALG);
            HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);

            std::wstring shape = GetComboBoxSelectedText(hShapeBox);
            std::wstring algorithm = GetComboBoxSelectedText(hAlgBox);
            std::wstring colorName = GetComboBoxSelectedText(hColorBox);

            color = shapeColorMap.count(colorName) ? shapeColorMap[colorName] : customColor;

            if (!hDrawingWindow || !IsWindow(hDrawingWindow)) {
                hDrawingWindow = CreateDrawingWindow(hwnd);
            }

            // Now you can call your draw function:
            // drawShape(hDrawingWindow, shape, algorithm, color);
            MessageBox(hwnd, (L"Drawing: " + shape + L" using " + algorithm + L" in color " + colorName).c_str(), L"Draw", MB_OK);
        }

        // Mouse change
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_CURSOR) {
            HWND hMouseBox = GetDlgItem(hwnd, ID_COMBO_CURSOR);
            std::wstring cursorName = GetComboBoxSelectedText(hMouseBox);

            if (cursorName == L"Arrow")
                currentCursor = LoadCursor(NULL, IDC_ARROW);
            else if (cursorName == L"Cross")
                currentCursor = LoadCursor(NULL, IDC_CROSS);
            else if (cursorName == L"Hand")
                currentCursor = LoadCursor(NULL, IDC_HAND);
            else if (cursorName == L"IBeam")
                currentCursor = LoadCursor(NULL, IDC_IBEAM);
            else
                currentCursor = LoadCursor(NULL, IDC_ARROW); // Fallback

            // Force both windows to refresh their cursors
            SetCursor(currentCursor); // Immediately update if hovering
            InvalidateRect(hwnd, NULL, FALSE);
            if (hDrawingWindow && IsWindow(hDrawingWindow))
                InvalidateRect(hDrawingWindow, NULL, FALSE);
        }

        // Clear btn
        if (LOWORD(wParam) == ID_BTN_CLEAR) {
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                InvalidateRect(hDrawingWindow, NULL, TRUE); // Repaints, triggering WM_PAINT
            }
            break;
        }
        // Bkg white btn
        if (LOWORD(wParam) == ID_BTN_WHITE_BG) {
            drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // White background
            if (hDrawingWindow && IsWindow(hDrawingWindow)) {
                InvalidateRect(hDrawingWindow, NULL, TRUE); // Refresh the window
            }
            //InvalidateRect(hDrawingWindow, NULL, TRUE);
            break;
        }

        break;
    }
    
    case WM_SETCURSOR: {
        SetCursor(currentCursor);
        return TRUE;
    }
    case WM_DESTROY:
        DeleteObject(hFontLarge);
        DeleteObject(hFontMedium);
        DeleteObject(hFontSmall);
        DeleteObject(drawingBackgroundBrush);
        PostQuitMessage(0);

        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MainMenuWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, NULL,
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
