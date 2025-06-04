#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "filling.h"
#include "Ellipse.h"
#include "clipping.h"

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

HFONT hFontLarge, hFontMedium, hFontSmall;
HCURSOR currentCursor = LoadCursor(NULL, IDC_ARROW);
HBRUSH drawingBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
HWND hDrawingWindow = NULL;
std::wstring algorithm = L"";
std::wstring shape = L"";
COLORREF color;
COLORREF customColor = RGB(0, 0, 0);
COLORREF bcolor = RGB(0, 0, 0);
std::vector<Point> clickPoints;
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



void drawUI(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    hFontLarge = CreateFont(-18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    hFontMedium = CreateFont(-16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    HWND hRibbon = CreateWindowEx(0, L"STATIC", L"Graphics GUI", WS_CHILD | WS_VISIBLE, 10, 10, 200, 30, hwnd, NULL, NULL, NULL);
    SendMessage(hRibbon, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hWelcome = CreateWindowEx(0, L"STATIC", L"Welcome to Our Drawing Program!", WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 60, 590, 30, hwnd, (HMENU)ID_STATIC_WELCOME, NULL, NULL);
    SendMessage(hWelcome, WM_SETFONT, (WPARAM)hFontLarge, TRUE);

    HWND hShapeLabel = CreateWindowEx(0, L"STATIC", L"Shape:", WS_CHILD | WS_VISIBLE, 80, 120, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessage(hShapeLabel, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeBox = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 120, 250, 200, hwnd, (HMENU)ID_COMBO_SHAPE, NULL, NULL);
    SendMessage(hShapeBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hAlgorithm = CreateWindowEx(0, L"STATIC", L"Algorithm:", WS_CHILD | WS_VISIBLE, 80, 150, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessage(hAlgorithm, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hAlgBox = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 150, 250, 200, hwnd, (HMENU)ID_COMBO_ALG, NULL, NULL);
    SendMessage(hAlgBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeColor = CreateWindowEx(0, L"STATIC", L"Shape Color:", WS_CHILD | WS_VISIBLE, 80, 190, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessage(hShapeColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hShapeColorBox = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 190, 250, 200, hwnd, (HMENU)ID_COMBO_COLOR, NULL, NULL);
    SendMessage(hShapeColorBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouse = CreateWindowEx(0, L"STATIC", L"Mouse Shape:", WS_CHILD | WS_VISIBLE, 80, 230, 120, 25, hwnd, NULL, NULL, NULL);
    SendMessage(hMouse, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hMouseBox = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 270, 230, 250, 200, hwnd, (HMENU)ID_COMBO_CURSOR, NULL, NULL);
    SendMessage(hMouseBox, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hBackground = CreateWindowEx(0, L"BUTTON", L"Change Background Color", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 168, 300, 270, 25, hwnd, (HMENU)ID_BTN_WHITE_BG, NULL, NULL);
    SendMessage(hBackground, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hCustomColor = CreateWindow(L"BUTTON", L"Custom Color", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER, 456, 300, 120, 25, hwnd, (HMENU)IDC_CUSTOM_COLOR_BUTTON, NULL, NULL);
    SendMessage(hCustomColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hCustomBColor = CreateWindow(L"BUTTON", L"Border Color", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CENTER | BS_VCENTER, 24, 300, 120, 25, hwnd, (HMENU)IDC_CUSTOM_BCOLOR_BUTTON, NULL, NULL);
    SendMessage(hCustomBColor, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hClear = CreateWindowEx(0, L"BUTTON", L"Clear Screen", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 24, 350, 120, 25, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
    SendMessage(hClear, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hDrawButton = CreateWindowEx(0, L"BUTTON", L"Draw", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 168, 350, 120, 25, hwnd, (HMENU)ID_BTN_DRAW, NULL, NULL);
    SendMessage(hDrawButton, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hSave = CreateWindowEx(0, L"BUTTON", L"Save to File", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 312, 350, 120, 25, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
    SendMessage(hSave, WM_SETFONT, (WPARAM)hFontMedium, TRUE);

    HWND hLoad = CreateWindowEx(0, L"BUTTON", L"Load from File", WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER, 456, 350, 120, 25, hwnd, (HMENU)ID_BTN_LOAD, NULL, NULL);
    SendMessage(hLoad, WM_SETFONT, (WPARAM)hFontMedium, TRUE);
}

void populateShapeAlgorithm(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Line");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Rectangle Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Convex Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Non-Convex Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Flood Fill");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Cardinal Spline Curve");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Ellipse");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Rectangle Clipping");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Square Clipping");
    SendMessage(hShapeBox, CB_ADDSTRING, 0, (LPARAM)L"Circle Clipping");
    SendMessage(hShapeBox, CB_SETCURSEL, 0, 0);
    SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_COMBO_SHAPE, CBN_SELCHANGE), (LPARAM)hShapeBox);
}

void populateColorList(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
    for (const auto& pair : shapeColorMap) {
        SendMessage(hColorBox, CB_ADDSTRING, 0, (LPARAM)pair.first.c_str());
    }
    SendMessage(hColorBox, CB_SETCURSEL, 0, 0);
}

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
}

std::wstring GetComboBoxSelectedText(HWND comboBox) {
    int sel = (int)SendMessage(comboBox, CB_GETCURSEL, 0, 0);
    if (sel == CB_ERR) return L"";
    wchar_t buffer[256];
    SendMessage(comboBox, CB_GETLBTEXT, sel, (LPARAM)buffer);
    return std::wstring(buffer);
}

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

    switch (msg) {
    case WM_CREATE: {
        HWND hQuarterCombo = CreateWindow(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL, 10, 10, 100, 100, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        SendMessage(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"1");
        SendMessage(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"2");
        SendMessage(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"3");
        SendMessage(hQuarterCombo, CB_ADDSTRING, 0, (LPARAM)L"4");
        SendMessage(hQuarterCombo, CB_SETCURSEL, 0, 0);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1 && HIWORD(wParam) == CBN_SELCHANGE) {
            HWND hQuarterCombo = GetDlgItem(hwnd, 1);
            selectQuarter = SendMessage(hQuarterCombo, CB_GETCURSEL, 0, 0) + 1;
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
            clickPoints.push_back(Point(X1, Y1));
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
            clickPoints.push_back(Point(X1, Y1));
            clickCount++;
        }
        break;
    }
    case WM_LBUTTONUP: {
        hdc = GetDC(hwnd);
        X2 = LOWORD(lParam);
        Y2 = HIWORD(lParam);
        int X = X2;
        int Y = Y2;
        R = round(sqrt((X - Xc) * (X - Xc) + (Y - Yc) * (Y - Yc)));

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
                int lengthSq = max(abs(dx), abs(dy));
                if (dx < 0) X2 = X1 - lengthSq; else X2 = X1 + lengthSq;
                if (dy < 0) Y2 = Y1 - lengthSq; else Y2 = Y1 + lengthSq;
                DrawSquare(hdc, X1, Y1, lengthSq, bcolor);
                FillingSquareWithHermiteCurves(hdc, X1, Y1, lengthSq, 5, bcolor, color);
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
                std::vector<Point> testConvex = { Point(100, 100), Point(200, 100), Point(150, 200), Point(170, 250), Point(90, 250) };
                CurveFill(hdc, testConvex, color);
            }
            clickPoints.clear();
            clickCount = 0;
        }
        else if (shape == L"Non-Convex Fill") {
            if (algorithm == L"Polygon Fill") {
                std::vector<Point> testPolygon = { Point(100, 100), Point(200, 100), Point(150, 200), Point(170, 250), Point(90, 250) };
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
                std::vector<Point> points;
                for (const auto& p : clickPoints) points.push_back(Point((double)p.X, (double)p.Y));
                CardinalSplineCurve(hdc, points, 0.5, color);
                points.clear();
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND CreateDrawingWindow(HWND parent) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = DrawingWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"DrawingWindowClass";
    RegisterClass(&wc);

    return CreateWindowEx(0, L"DrawingWindowClass", L"Drawing Canvas", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 700, 100, 600, 500, parent, NULL, GetModuleHandle(NULL), NULL);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const int titleBarHeight = 40;
    static RECT closeButtonRect = { 560, 5, 590, 35 };

    switch (msg) {
    case WM_ERASEBKGND: {
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
        FillRect(hdc, &clientRect, drawingBackgroundBrush);
        HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
        RECT ribbonRect = { 0, 0, 600, titleBarHeight };
        FillRect(hdc, &ribbonRect, hBlack);
        DeleteObject(hBlack);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        TextOut(hdc, 10, 10, L"Graphics GUI", lstrlen(L"Graphics GUI"));
        DrawText(hdc, L"X", 1, &closeButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, 0, 0, 600, 400);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(hPen);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hStatic = (HWND)lParam;
        int id = GetDlgCtrlID(hStatic);
        wchar_t text[100];
        GetWindowText(hStatic, text, 100);
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
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (y < titleBarHeight) {
            if (x >= closeButtonRect.left && x <= closeButtonRect.right && y >= closeButtonRect.top && y <= closeButtonRect.bottom) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            else {
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
        break;
    }
    case WM_COMMAND: {
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_SHAPE) {
            HWND hShapeBox = GetDlgItem(hwnd, ID_COMBO_SHAPE);
            HWND hAlgBox = GetDlgItem(hwnd, ID_COMBO_ALG);
            int index = SendMessage(hShapeBox, CB_GETCURSEL, 0, 0);
            wchar_t selectedShape[100];
            SendMessage(hShapeBox, CB_GETLBTEXT, index, (LPARAM)selectedShape);
            SendMessage(hAlgBox, CB_RESETCONTENT, 0, 0);
            for (const auto& alg : shapeAlgorithmMap[selectedShape]) {
                SendMessage(hAlgBox, CB_ADDSTRING, 0, (LPARAM)alg.c_str());
            }
            SendMessage(hAlgBox, CB_SETCURSEL, 0, 0);
        }
        if (LOWORD(wParam) == IDC_CUSTOM_COLOR_BUTTON) {
            CHOOSECOLOR cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = customColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc)) {
                customColor = cc.rgbResult;
                shapeColorMap[L"Custom"] = customColor;
                HWND hColorBox = GetDlgItem(hwnd, ID_COMBO_COLOR);
                int index = (int)SendMessage(hColorBox, CB_FINDSTRINGEXACT, -1, (LPARAM)L"Custom");
                if (index == CB_ERR) index = (int)SendMessage(hColorBox, CB_ADDSTRING, 0, (LPARAM)L"Custom");
                SendMessage(hColorBox, CB_SETCURSEL, index, 0);
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
            MessageBox(hwnd, (L"Drawing: " + shape + L" using " + algorithm + L" in color " + colorName).c_str(), L"Draw", MB_OK);
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_COMBO_CURSOR) {
            HWND hMouseBox = GetDlgItem(hwnd, ID_COMBO_CURSOR);
            std::wstring cursorName = GetComboBoxSelectedText(hMouseBox);
            if (cursorName == L"Arrow") currentCursor = LoadCursor(NULL, IDC_ARROW);
            else if (cursorName == L"I-Beam") currentCursor = LoadCursor(NULL, IDC_IBEAM);
            else if (cursorName == L"Cross") currentCursor = LoadCursor(NULL, IDC_CROSS);
            else if (cursorName == L"Hand") currentCursor = LoadCursor(NULL, IDC_HAND);
            else if (cursorName == L"Wait") currentCursor = LoadCursor(NULL, IDC_WAIT);
            else if (cursorName == L"No") currentCursor = LoadCursor(NULL, IDC_NO);
            else if (cursorName == L"Size All") currentCursor = LoadCursor(NULL, IDC_SIZEALL);
            else if (cursorName == L"Size NW-SE") currentCursor = LoadCursor(NULL, IDC_SIZENWSE);
            else if (cursorName == L"Size NE-SW") currentCursor = LoadCursor(NULL, IDC_SIZENESW);
            else if (cursorName == L"Size WE") currentCursor = LoadCursor(NULL, IDC_SIZEWE);
            else if (cursorName == L"Size NS") currentCursor = LoadCursor(NULL, IDC_SIZENS);
            else currentCursor = LoadCursor(NULL, IDC_ARROW);
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
            }
        }
        if (LOWORD(wParam) == ID_BTN_WHITE_BG) {
            CHOOSECOLOR cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = customColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc)) {
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
            CHOOSECOLOR cc;
            static COLORREF acrCustClr[16];
            ZeroMemory(&cc, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = bcolor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc)) {
                bcolor = cc.rgbResult;
            }
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

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, NULL, WS_POPUP | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
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