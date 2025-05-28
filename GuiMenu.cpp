#include <windows.h>

#define ID_COMBO_SHAPE      101
#define ID_COMBO_COLOR      102
#define ID_COMBO_CURSOR     103
#define ID_BTN_WHITE_BG     104
#define ID_BTN_CLEAR        105
#define ID_BTN_SAVE         106
#define ID_BTN_LOAD         107
#define ID_BTN_DRAW         108
#define ID_STATIC_WELCOME   1000
HFONT hFontLarge, hFontMedium, hFontSmall;



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const int titleBarHeight = 40;
    static RECT closeButtonRect = { 560, 5, 590, 35 }; // Custom close button

    switch (msg) {
    case WM_CREATE: {

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
            150, 70, 320, 30,
            hwnd, (HMENU)ID_STATIC_WELCOME, NULL, NULL);
        SendMessage(hWelcome, WM_SETFONT, (WPARAM)hFontLarge, TRUE);


        HWND hShapeLabel = CreateWindowEx(0, L"STATIC", L"Shape:",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            50, 130, 120, 25,
            hwnd, NULL, NULL, NULL);
        SendMessage(hShapeLabel, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hShapeBox = CreateWindowEx(0, L"COMBOBOX", NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            270, 130, 250, 200,
            hwnd, (HMENU)ID_COMBO_SHAPE, NULL, NULL);
        SendMessage(hShapeBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hShapeColor = CreateWindowEx(0, L"STATIC", L"Shape Color:",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            50, 170, 120, 25,
            hwnd, NULL, NULL, NULL);
        SendMessage(hShapeColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hShapeColorBox = CreateWindowEx(0, L"COMBOBOX", NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            270, 170, 250, 200,
            hwnd, (HMENU)ID_COMBO_COLOR, NULL, NULL);
        SendMessage(hShapeColorBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hMouse = CreateWindowEx(0, L"STATIC", L"Mouse Shape:",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            50, 160+50, 120, 25,
            hwnd, NULL, NULL, NULL);
        SendMessage(hMouse, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hMouseBox = CreateWindowEx(0, L"COMBOBOX", NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            270, 210, 250, 200,
            hwnd, (HMENU)ID_COMBO_CURSOR, NULL, NULL);
        SendMessage(hMouseBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hBackground = CreateWindowEx(0, L"BUTTON", L"Change Background to White",
            WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
            180, 275, 240, 25,
            hwnd, (HMENU)ID_BTN_WHITE_BG, NULL, NULL);
        SendMessage(hBackground, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hClear = CreateWindowEx(0, L"BUTTON", L"Clear Screen",
            WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
            24, 325, 120, 25,
            hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
        SendMessage(hClear, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hDrawButton = CreateWindowEx(0, L"BUTTON", L"Draw",
            WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
            168, 325, 120, 25,
            hwnd, (HMENU)ID_BTN_DRAW, NULL, NULL);
        SendMessage(hDrawButton, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hSave = CreateWindowEx(0, L"BUTTON", L"Save to File",
            WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
            312, 325, 120, 25,
            hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
        SendMessage(hSave, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        HWND hLoad = CreateWindowEx(0, L"BUTTON", L"Load from File",
            WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER,
            456, 325, 120, 25,
            hwnd, (HMENU)ID_BTN_LOAD, NULL, NULL);
        SendMessage(hLoad, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

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

        return DefWindowProc(hwnd, WM_CTLCOLORSTATIC, wParam, lParam);
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

    case WM_DESTROY:
        PostQuitMessage(0);
        DeleteObject(hFontLarge);
        DeleteObject(hFontMedium);
        DeleteObject(hFontSmall);

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
