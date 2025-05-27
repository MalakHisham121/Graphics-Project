#include <windows.h>
#include <iostream>
#include<vector>
#include <math.h>
// Author: Malak Hisham
// use following commands to compile and run the code
// g++ -o clipping clipping.cpp -mwindows
// ./clipping
using namespace std;

struct Linepoint {
    double x, y;
    Linepoint(double a, double b) : x(a), y(b) {}
};



union code {
    unsigned whole : 4;
    struct {
        unsigned left : 1, right : 1, bottom : 1, top : 1;
    };
};

struct Rectangl {
    double X_min, X_max, Y_min, Y_max;
};

code SetCode(Linepoint & p, Rectangl & R) {
    code c;
    c.whole = 0;
    if (p.x < R.X_min)
        c.left = 1;
    else if (p.x > R.X_max)
        c.right = 1;
    if (p.y < R.Y_min)
        c.bottom = 1;
    else if (p.y > R.Y_max)
        c.top = 1;
    return c;
}
struct Poly{
    vector<Linepoint> vertices;
    vector<code> verticesPos;

};


struct Line {
    Linepoint p1, p2;
};
struct Circle {
    double xc, yc, r;
};

void LineClipping(Rectangl& R, Linepoint& p1, Linepoint& p2, code& code1, code& code2) {
    // use cohen sutherland algorithem
    double slope = (p2.x - p1.x) ? (p2.y - p1.y) / (p2.x - p1.x) : 0;
    bool isVertical = (p2.x == p1.x);

    while ((code1.whole || code2.whole) && !(code1.whole & code2.whole)) {
        if (code1.whole) {
            if (code1.left) {
                p1.y = p1.y + slope * (R.X_min - p1.x);
                p1.x = R.X_min;
            }
            else if (code1.right) {
                p1.y = p1.y + slope * (R.X_max - p1.x);
                p1.x = R.X_max;
            }
            else if (code1.bottom) {
                if (!isVertical) {
                    p1.x = p1.x + (1.0 / slope) * (R.Y_min - p1.y);
                }
                p1.y = R.Y_min;
            }
            else if (code1.top) {
                if (!isVertical) {
                    p1.x = p1.x + (1.0 / slope) * (R.Y_max - p1.y);
                }
                p1.y = R.Y_max;
            }
            code1 = SetCode(p1, R);
        }
        else if (code2.whole) {
            if (code2.left) {
                p2.y = p2.y + slope * (R.X_min - p2.x);
                p2.x = R.X_min;
            }
            else if (code2.right) {
                p2.y = p2.y + slope * (R.X_max - p2.x);
                p2.x = R.X_max;
            }
            else if (code2.bottom) {
                if (!isVertical) {
                    p2.x = p2.x + (1.0 / slope) * (R.Y_min - p2.y);
                }
                p2.y = R.Y_min;
            }
            else if (code2.top) {
                if (!isVertical) {
                    p2.x = p2.x + (1.0 / slope) * (R.Y_max - p2.y);
                }
                p2.y = R.Y_max;
            }
            code2 = SetCode(p2, R);
        }
    }
}
vector<Linepoint> check(int ref, Linepoint &p1, Linepoint & p2, bool greaterisInner,bool X)
{
    vector<Linepoint> result;
    double slope = (p2.x != p1.x) ? (p2.y - p1.y) / (p2.x - p1.x) : 0;
bool isVertical = (p2.x == p1.x);
if (greaterisInner) {
    if (X) { 
        if (p1.x >= ref && p2.x >= ref) { // Both inside
            result.push_back(p2);
        }
        else if (p1.x >= ref && p2.x < ref) { // p1 inside, p2 outside
            double x = ref;
            double y = p1.y + slope * (ref - p1.x);
            result.push_back(Linepoint(x, y));
        }
        else if (p1.x < ref && p2.x >= ref) { // p1 outside, p2 inside
            double x = ref;
            double y = p1.y + slope * (ref - p1.x);
            result.push_back(Linepoint(x, y));
            result.push_back(p2);
        }
        
    }
    else { 
        if (p1.y >= ref && p2.y >= ref) {
            result.push_back(p2);
        }
        else if (p1.y >= ref && p2.y < ref) {
            double y = ref;
            double x = isVertical ? p1.x : p1.x + (1.0 / slope) * (ref - p1.y);
            result.push_back(Linepoint(x, y));
        }
        else if (p1.y < ref && p2.y >= ref) {
            double y = ref;
            double x = isVertical ? p1.x : p1.x + (1.0 / slope) * (ref - p1.y);
            result.push_back(Linepoint(x, y));
            result.push_back(p2);
        }
    
    }
}else { // greaterisInner = false
    if (X) { 
        if (p1.x <= ref && p2.x <= ref) {
            result.push_back(p2);
        }
        else if (p1.x <= ref && p2.x > ref) {
            double x = ref;
            double y = p1.y + slope * (ref - p1.x);
            result.push_back(Linepoint(x, y));
        }
        else if (p1.x > ref && p2.x <= ref) {
            double x = ref;
            double y = p1.y + slope * (ref - p1.x);
            result.push_back(Linepoint(x, y));
            result.push_back(p2);
        }
    }
    else { 
        if (p1.y <= ref && p2.y <= ref) {
            result.push_back(p2);
        }
        else if (p1.y <= ref && p2.y > ref) {
            double y = ref;
            double x = isVertical ? p1.x : p1.x + (1.0 / slope) * (ref - p1.y);
            result.push_back(Linepoint(x, y));
        }
        else if (p1.y > ref && p2.y <= ref) {
            double y = ref;
            double x = isVertical ? p1.x : p1.x + (1.0 / slope) * (ref - p1.y);
            result.push_back(Linepoint(x, y));
            result.push_back(p2);
        }
    }
}
return result;
}



void RectangleClipping(HDC hdc, int type, Rectangl R, Linepoint p1, Linepoint p2, Poly poly ={}) {
    if (type == 0) {
        code c = SetCode(p1, R);
        if (!c.whole) {
            SetPixel(hdc, round(p1.x), round(p1.y), RGB(0, 0, 255)); // Blue point
        }
    }
    else if (type == 1) {
        code code1 = SetCode(p1, R);
        code code2 = SetCode(p2, R);
        // call line drawing algorithems here 

        // Draw original line in red
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, round(p1.x), round(p1.y), NULL);
        LineTo(hdc, round(p2.x), round(p2.y));
        DeleteObject(hPen);

        // Clip the line
        LineClipping(R, p1, p2, code1, code2);

        // Draw clipped line in blue if accepted
        if (!(code1.whole || code2.whole)) {
            hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
            SelectObject(hdc, hPen);
            MoveToEx(hdc, round(p1.x), round(p1.y), NULL);
            LineTo(hdc, round(p2.x), round(p2.y));
            DeleteObject(hPen);
        } 
    }
    else if (type == 2) {
if (type == 2) {
    vector<Linepoint> polygon = poly.vertices;
    if (polygon.empty() || polygon.size() < 3) return;

    // Draw original polygon in red
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    SelectObject(hdc, hPen);
    MoveToEx(hdc, round(polygon[0].x), round(polygon[0].y), NULL);
    for (size_t i = 1; i < polygon.size(); ++i) {
        LineTo(hdc, round(polygon[i].x), round(polygon[i].y));
    }
    LineTo(hdc, round(polygon[0].x), round(polygon[0].y));
    DeleteObject(hPen);

    vector<Linepoint> input = polygon;
    vector<Linepoint> output;
    // Boundary order: left, right, bottom, top
    for (int boundary = 0; boundary < 4; ++boundary) {
        output.clear();
        int ref = (boundary == 0) ? R.X_min : (boundary == 1) ? R.X_max :
                  (boundary == 2) ? R.Y_min : R.Y_max;
        bool greaterisInner = (boundary == 0 || boundary == 2);
        bool X = (boundary == 0 || boundary == 1);
        for (size_t i = 0; i < input.size(); ++i) {
            Linepoint p1 = input[i];
            Linepoint p2 = input[(i + 1) % input.size()];
            vector<Linepoint> edgeResult = check(ref, p1, p2, greaterisInner, X);
            for (const auto& point : edgeResult) {
                output.push_back(point);
            }
        }
        input = output;
    }

    // Draw clipped polygon in blue
    hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(hdc, hPen);
    if (!output.empty()) {
        MoveToEx(hdc, round(output[0].x), round(output[0].y), NULL);
        for (size_t i = 1; i < output.size(); ++i) {
            LineTo(hdc, round(output[i].x), round(output[i].y));
        }
        LineTo(hdc, round(output[0].x), round(output[0].y));
    }
    DeleteObject(hPen);
}    }
}

vector<Linepoint> findIntersections(Linepoint p1, Linepoint p2, Circle C) {
    vector<Linepoint> intersections;
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double a = p1.x - C.xc;
    double b = p1.y - C.yc;
    double A = dx * dx + dy * dy;
    double B = 2 * (a * dx + b * dy);
    double C_val = a * a + b * b - C.r * C.r;
    double D = B * B - 4 * A * C_val;
    if (D < 0) return intersections; 
    double t1 = (-B + sqrt(D)) / (2 * A);
    double t2 = (-B - sqrt(D)) / (2 * A);
    if (0 <= t1 && t1 <= 1) {
        intersections.push_back(Linepoint(p1.x + t1 * dx, p1.y + t1 * dy));
    }
    if (0 <= t2 && t2 <= 1 && D > 0) {
        intersections.push_back(Linepoint(p1.x + t2 * dx, p1.y + t2 * dy));
    }
    return intersections;
}

void drawCircle(HDC hdc, Circle C) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);
    // Approximate circle with points
    int numPoints = 100;
    for (int i = 0; i <= numPoints; ++i) {
        double theta = 2 * M_PI * i / numPoints;
        double x = C.xc + C.r * cos(theta);
        double y = C.yc + C.r * sin(theta);
        if (i == 0) MoveToEx(hdc, round(x), round(y), NULL);
        else LineTo(hdc, round(x), round(y));
    }
    DeleteObject(hPen);
}

void CircleClipping(HDC hdc, int type, Circle C, Linepoint & p1, Linepoint & p2) {
    int d1 = (p1.x - C.xc) * (p1.x - C.xc) + (p1.y - C.yc) * (p1.y - C.yc);
    int d2 = (p2.x - C.xc) * (p2.x - C.xc) + (p2.y - C.yc) * (p2.y - C.yc);
    int rsqured = C.r * C.r;
    bool draw = 0;
    if(d1<= rsqured&&d2<=rsqured){
        draw = 1;
    }
    else if(d1<=rsqured){
auto intersections = findIntersections(p1, p2, C);
        if (!intersections.empty()) {
            p2 = intersections[0];
            draw = 1;
        }
    }
    else if(d2<=rsqured){
auto intersections = findIntersections(p1, p2, C);
        if (!intersections.empty()) {
            p1 = intersections[0];
            draw = 1;
        }
    }
    else{
      auto intersections = findIntersections(p1, p2, C);
        if (intersections.size() == 2) {
            p1 = intersections[0];
            p2 = intersections[1];
            draw=1;
        
        }  
    }

    // Use Another done circle algorithems 
    drawCircle(hdc, C);

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    SelectObject(hdc, hPen);
    MoveToEx(hdc, round(p1.x), round(p1.y), NULL);
    LineTo(hdc, round(p2.x), round(p2.y));
    DeleteObject(hPen);

    if (draw) {
        hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, round(p1.x), round(p1.y), NULL);
        LineTo(hdc, round(p2.x), round(p2.y));
        DeleteObject(hPen);
    }
}


int testCase = 2;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Define the clipping rectangle
        Rectangl R = {100.0, 300.0, 100.0, 300.0}; // X_min, X_max, Y_min, Y_max

        // Draw the clipping rectangle in black
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, round(R.X_min), round(R.Y_min), NULL);
        LineTo(hdc, round(R.X_max), round(R.Y_min));
        LineTo(hdc, round(R.X_max), round(R.Y_max));
        LineTo(hdc, round(R.X_min), round(R.Y_max));
        LineTo(hdc, round(R.X_min), round(R.Y_min));
        DeleteObject(hPen);

        // Test cases
        static int testCase = 3; // Moved from global to avoid redefinition issues
        if (testCase == 0) {
            // Point clipping
            Linepoint p1(150.0, 150.0); // Inside
            Linepoint p2(50.0, 50.0);   // Outside
            RectangleClipping(hdc, 0, R, p1, p2);
            RectangleClipping(hdc, 0, R, p2, p2);
        }
        else if (testCase == 1) {
            // Line clipping test cases
            Linepoint p1(120.0, 120.0), p2(280.0, 280.0); // Fully inside
            RectangleClipping(hdc, 1, R, p1, p2);

            p1 = {50.0, 50.0}, p2 = {200.0, 200.0}; // Partially inside
            RectangleClipping(hdc, 1, R, p1, p2);

            p1 = {50.0, 150.0}, p2 = {80.0, 150.0}; // Fully outside
            RectangleClipping(hdc, 1, R, p1, p2);

            p1 = {150.0, 50.0}, p2 = {150.0, 350.0}; // Vertical line
            RectangleClipping(hdc, 1, R, p1, p2);
        }
        else if (testCase == 2) {
            Poly polygon;
            polygon.vertices= {
                Linepoint(50.0, 50.0),
                Linepoint(200.0, 50.0),
                Linepoint(250.0, 200.0),
                Linepoint(150.0, 250.0),
                Linepoint(50.0, 200.0)};

            RectangleClipping(hdc, 2, R, Linepoint(0, 0), Linepoint(0, 0), polygon);        }
        if (testCase == 3) {
        Circle C = {200.0, 200.0, 100.0}; 
        Linepoint p1(500.0, 150.0), p2(250.0, 250.0); 
        CircleClipping(hdc, 1, C, p1, p2);
        p1 = Linepoint(100.0, 100.0), p2 = Linepoint(300.0, 300.0); 
        CircleClipping(hdc, 1, C, p1, p2);
        p1 = Linepoint(50.0, 50.0), p2 = Linepoint(100.0, 50.0); 
        CircleClipping(hdc, 1, C, p1, p2);
    }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        if (wParam == '0') testCase = 0;
        else if (wParam == '1') testCase = 1;
        else if (wParam == '2') testCase = 2;
        else if (wParam == '3') testCase = 3;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"ClippingWindow"; // Fixed array notation
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,                        // Extended style
        CLASS_NAME,               // Class name
        L"Clipping Algorithm Test", // Window title
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}