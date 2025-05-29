#include <windows.h>
#include <math.h>

// Author : Malak Hisham
// to run thr code use following commands 
// 

void Draw4Pixel(HDC hdc, int x, int y, int Xc, int Yc, COLORREF c) {
    SetPixel(hdc, Xc + x, Yc + y, c);
    SetPixel(hdc, Xc - x, Yc + y, c);
    SetPixel(hdc, Xc + x, Yc - y, c);
    SetPixel(hdc, Xc - x, Yc - y, c);
}

void DirectEllipse(HDC hdc, int a, int b, int Xc, int Yc) {
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;
    for (int x = 0; x <= a; x++) {
        long long y = (long long)b * sqrt(1.0 - (double)(x * x) / a2);
        Draw4Pixel(hdc, x, (int)y, Xc, Yc, RGB(255, 0, 0)); // Red
    }
}

void PolarEllipse(HDC hdc, int a, int b, int Xc, int Yc) {
    for (double theta = 0; theta <= M_PI / 2; theta += 0.01) {
        int x = (int)(a * cos(theta));
        int y = (int)(b * sin(theta));
        Draw4Pixel(hdc, x, y, Xc, Yc, RGB(0, 255, 0)); // Green
    }
}

void MidPointEllipse(HDC hdc, int a, int b, int Xc, int Yc) {
    int x = 0, y = b;
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;
    long long two_a2 = 2 * a2;
    long long two_b2 = 2 * b2;

    // Region 1: Slope < 1
    long long d = b2 - a2 * b + a2 / 4;
    while (b2 * x <= a2 * y) { // b^2 * x <= a^2 * y
        Draw4Pixel(hdc, x, y, Xc, Yc, RGB(0, 0, 255)); // Blue
        x++;
        if (d < 0) {
            d += two_b2 * x + b2;
        } else {
            y--;
            d += two_b2 * x + b2 - two_a2 * y;
        }
    }

    // Region 2: Slope >= 1
    d = b2 * (x * x + x) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        Draw4Pixel(hdc, x, y, Xc, Yc, RGB(0, 0, 255)); // Blue
        y--;
        if (d < 0) {
            x++;
            d += two_b2 * x - two_a2 * y + a2;
        } else {
            d += -two_a2 * y + a2;
        }
    }
}

void EllipseDrawing(HDC hdc, int algo, int Xc, int Yc) {
    int a = 250; // Semi-major axis
    int b = 150; // Semi-minor axis
    switch (algo) {
        case 1:
            DirectEllipse(hdc, a, b, Xc, Yc);
            break;
        case 2:
            PolarEllipse(hdc, a, b, Xc, Yc);
            break;
        case 3:
            MidPointEllipse(hdc, a, b, Xc, Yc);
            break;
        default:
            break;
    }
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int algo = 0; // Current algorithm (0: none, 1: Direct, 2: Polar, 3: Midpoint)
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    algo = 1; // Direct
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case 2:
                    algo = 2; // Polar
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case 3:
                    algo = 3; // Midpoint
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            if (algo != 0) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                hdc = GetDC(hwnd);
                EllipseDrawing(hdc, algo, x, y);
                ReleaseDC(hwnd, hdc);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* ClassName = "EllipseWindow";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = ClassName;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(ClassName, "Ellipse Drawing", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                            NULL, NULL, hInstance, NULL);

    // Create menu
    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, 1, "Direct Ellipse");
    AppendMenu(hSubMenu, MF_STRING, 2, "Polar Ellipse");
    AppendMenu(hSubMenu, MF_STRING, 3, "Midpoint Ellipse");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, "Algorithms");
    SetMenu(hwnd, hMenu);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam; 
}