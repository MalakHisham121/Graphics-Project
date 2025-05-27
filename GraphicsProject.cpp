// Name : Shahd Elnassag
// ID : 20220533

#include <Windows.h>
#include <cmath>
#include <algorithm>

using namespace std;

int Round(double x)
{
    return (int)(x + 0.5);
}

// 9. Implement line algorithms [DDA, Midpoint and parametric]
// 1. DDA
void DrawLineDDA(HDC hdc, int X1, int Y1, int X2, int Y2, COLORREF c) {

    int dx = X2 - X1; int dy = Y2 - Y1;
    SetPixel(hdc, X1, Y1, c);

    if (abs(dx) >= abs(dy)) {
        double slope = (double) dy / dx;
        int X = X1;
        double Y = Y1;
        while (X < X2) {
            X++;
            Y += slope;
            SetPixel(hdc, X, Round(Y), c);

        }
    }
    else {
        double slopeInverse = (double) dx / dy;
        int Y = Y1;
        double X = X1;
        while (Y < Y2) {
            Y++;
            X += slopeInverse;
            SetPixel(hdc,Round(X), Y, c);

        }

    }
}

// 2. Midpoint Bresenham algorithm
void DrawLineMidpoint(HDC hdc, int X1, int Y1, int X2, int Y2, COLORREF c)
{
    int dx = abs(X2 - X1);
    int dy = abs(Y2 - Y1);

    int X = X1;
    int Y = Y1;
    // increment with correct direction
    int x_inc = (X2 > X1) ? 1 : -1;
    int y_inc = (Y2 > Y1) ? 1 : -1;

    SetPixel(hdc, X, Y, c);

    if (dx > dy) {
        int d = 2 * dy - dx;
        int d1 = 2 * dy;
        int d2 = 2 * (dy - dx);

        while (X != X2) {
            if (d < 0)
                d += d1;
            else {
                d += d2;
                Y += y_inc;
            }
            X += x_inc;
            SetPixel(hdc, X, Y, c);
        }
    }
    else {
        int d = 2 * dx - dy;
        int d1 = 2 * dx;
        int d2 = 2 * (dx - dy);

        while (Y != Y2) {
            if (d < 0)
                d += d1;
            else {
                d += d2;
                X += x_inc;
            }
            Y += y_inc;
            SetPixel(hdc, X, Y, c);
        }
    }
}

// 3. Parametric
void DrawLineParametric(HDC hdc, int X1, int Y1, int X2, int Y2, COLORREF c) {

    int dx = X2 - X1; int dy = Y2 - Y1;
    double t;
    for (t = 0.0; t <= 1.0; t += 0.001) {
        int x = round(X1 + t * dx);
        int y = round(Y1 + t * dy);
        SetPixel(hdc, x, y, c);
    }

}

// 4. Direct Method
void DrawLineDirect(HDC hdc, int X1, int Y1, int X2, int Y2, COLORREF c) {

    int dx = X2 - X1; int dy = Y2 - Y1;
    if (abs(dx) >= abs(dy)) {
        double slope = (double) dy / dx;
        int X = X1;
        SetPixel(hdc, X1, Y1, c);
        while (X < X2) {
            X++;
            int Y = Round(slope * (X - X1) + Y1);
            SetPixel(hdc, X, Y, c);

        }
    }
    else {
        double slopeInverse = (double) dx / dy;
        int Y = Y1;
        SetPixel(hdc, X1, Y1, c);
        while (Y < Y2) {
            Y++;
            int X = Round(slopeInverse * (Y - Y1) + X1);
            SetPixel(hdc, X, Y, c);

        }

    }
}

// 10. Implement Circle algorithms[Direct, Polar, iterative Polar, midpoint and modified Midpoint]

void Draw8Points(HDC hdc, int Xc, int Yc, int x, int y, COLORREF c) {
    SetPixel(hdc, Xc + x, Yc + y, c);
    SetPixel(hdc, Xc + x, Yc - y, c);
    SetPixel(hdc, Xc - x, Yc + y, c);
    SetPixel(hdc, Xc - x, Yc - y, c);
    SetPixel(hdc, Xc + y, Yc + x, c);
    SetPixel(hdc, Xc + y, Yc - x, c);
    SetPixel(hdc, Xc - y, Yc + x, c);
    SetPixel(hdc, Xc - y, Yc - x, c);
}

// 1. Direct Method
void DrawCircleDirect(HDC hdc, int Xc, int Yc, int R, COLORREF c)
{
    int x = 0; int y = R; 
    Draw8Points(hdc, Xc, Yc, x, y, c);
    while (x < y) {
        x++;
        y = Round(sqrt(R * R - x * x));
        Draw8Points(hdc, Xc, Yc, x, y, c);
    }
}

// 2. Polar Method
void DrawCirclePolar(HDC hdc, int Xc, int Yc, int R, COLORREF c)
{
    int x = R; int y = 0;
    Draw8Points(hdc, Xc, Yc, x, y, c);
    double theta = 0.0;
    double dtheta = 1.0 / R;
    while (x > y) {
        theta += dtheta; 
        x = Round(R * cos(theta));
        y = Round(R * sin(theta));

        Draw8Points(hdc, Xc, Yc, x, y, c);
    }
}

// 3. iterative Polar

 void DrawCircleIterativePolar(HDC hdc, int Xc, int Yc, int R, COLORREF c)
{
     double x = R; double y = 0;
     Draw8Points(hdc, Xc, Yc, Round(x), Round(y), c);
     double dtheta = 1.0 / R;
     double cosdtheta = cos(dtheta);
     double sindtheta = sin(dtheta);


     while (x > y) {
         double xdash = x * cosdtheta - y * sindtheta;
         double ydash = x * sindtheta + y * cosdtheta;
         x = xdash;
         y = ydash;

         Draw8Points(hdc, Xc, Yc, Round(x), Round(y), c);
     }
}

 // 4. Midpoint

 void DrawCircleMidpoint(HDC hdc, int Xc, int Yc, int R, COLORREF c)
 {
     int x = 0; int y = R; 
     Draw8Points(hdc, Xc, Yc, x, y, c);
     while (x < y) {
         int d = pow(x + 1, 2) + pow(y - 0.5, 2) - R * R;
         if (d < 0) {
             x++;
         }
         else {
             x++;
             y--;
         }
         Draw8Points(hdc, Xc, Yc, x, y, c);
     }
 }


// 5. Modified Midpoint Bresnham apply DDA
void DrawCircleModifiedMidpoint(HDC hdc, int Xc, int Yc, int R, COLORREF c)
{
    int x = 0; int y = R; int d = 1 - R;
    int d1 = 3; int d2 = 5 - 2 * R;
    Draw8Points(hdc, Xc, Yc, x, y, c);
    while (x < y) {
        if (d < 0) {
            d += d1;
            d2 += 2;
        }
        else {
            d += d2;
            d2 += 4;
            y--;
        }
        d1 += 2;
        x++;
        Draw8Points(hdc, Xc, Yc, x, y, c);
    }
}
// 15. Convex and Non-Convex Filling Algorithm
// Convex Polygon algorithm 
// Data Structure
#define MAXENTRIES 800
struct edgeTable{

    int xLeft;
    int xRight;
};
struct Point {

    double x, y;
    Point(double x = 0,double y = 0):x(x),y(y){}
};
// Utaliit Functions
//1.Initialize
void InitalizeTable(edgeTable tbl[]) {
    for (int i = 0; i < MAXENTRIES; i++) {
        tbl[i].xLeft = 100000;
        tbl[i].xRight = -100000;
    }
}
//2. Evalate edge
void ScanEdge(Point P1, Point P2, edgeTable tbl[]) {
    if (P1.y == P2.y) return;
    if (P1.y > P2.y) swap(P1, P2);
    int y = P1.y; int x = P1.x;
    double inversSlope = (P2.x - P1.x) / (P2.y - P1.y);
    while (y < P2.y) {
        if (x < tbl[y].xLeft) {
            tbl[y].xLeft = (int)ceil(x);
        }
        if (x > tbl[y].xRight) {
            tbl[y].xRight = (int)floor(x);
        }
        y++;
        x += inversSlope;
    }
}
//3.raw scan lines stored in the table
void Ploygon2Table(Point P[], int n, edgeTable tbl[]) {
    Point P1 = P[n - 1];
    for (int i = 0; i < n; i++)
    {
        Point P2 = P[i];
        ScanEdge(P1, P2, tbl);
        P1 = P[i];
    }

}
//4.draw scan lines to screen

void DrawPloygonLine(HDC hdc, edgeTable tbl[], COLORREF c) {
    for (int y = 0; y < MAXENTRIES; y++) {
        if (tbl[y].xLeft < tbl[y].xRight) {
            // Draw a visible horizontal line on this scanline
            DrawLineMidpoint(hdc, tbl[y].xLeft, y, tbl[y].xRight, y, c);
        }
    }
}

//5. main function of the algorithm
void ConvexFill(HDC hdc, Point P[], int n, COLORREF c) {
    edgeTable tbl[MAXENTRIES]; 
    InitalizeTable(tbl);
    Ploygon2Table(P, n, tbl);
    DrawPloygonLine(hdc, tbl, c);

}
// 16. Recursive and Non-Recursive Flood Fill
//1.Recursive Flood Fill
void FloodFill(HDC hdc, int x, int y, COLORREF bc, COLORREF fc) {
   COLORREF c = GetPixel(hdc, x, y);
   if (c == bc || c == fc) {
       return;
   }
   SetPixel(hdc, x, y, fc);

   FloodFill(hdc, x + 1, y, bc, fc);
   FloodFill(hdc, x - 1, y, bc, fc);
   FloodFill(hdc, x , y - 1, bc, fc);
   FloodFill(hdc, x , y + 1, bc, fc);


}


int X1, Y1, X2, Y2; // Global variables to store line endpoints
int Xc, Yc, R;  // Circle


LRESULT WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        // Store the first click (starting point of the line)
 /*      X1 = LOWORD(lp);
        Y1 = HIWORD(lp);
        break;*/

        // Assume center of circle
        Xc = LOWORD(lp);
        Yc = HIWORD(lp);
        break;

    case WM_LBUTTONUP:
    {
            // For circles
        int X = LOWORD(lp);
        int Y = HIWORD(lp);
        // calculate raduis
        R = Round(sqrt((X - Xc) * (X - Xc) + (Y - Yc) * (Y - Yc)));

        
        // Store the second click (ending point of the line)
        //X2 = LOWORD(lp);
        //Y2 = HIWORD(lp);


        // Draw the line
        hdc = GetDC(hwnd);

                     //// Lines /////

        //DrawLineDDA(hdc, X1, Y1, X2, Y2, RGB(255, 0, 0));
       // DrawLineMidpoint(hdc, X1, Y1, X2, Y2, RGB(0, 255, 0));
      //  DrawLineParametric(hdc, X1, Y1, X2, Y2, RGB(0, 0, 255));
     //   DrawLineDirect(hdc, X1, Y1, X2, Y2, RGB(0, 0, 0));

                //// Circles /////
        

      //  DrawCircleDirect(hdc, Xc, Yc, R, RGB(255, 0, 0));
      //  DrawCirclePolar(hdc, Xc, Yc, R, RGB(0, 255, 0));
      //  DrawCircleIterativePolar(hdc, Xc, Yc, R, RGB(0, 0, 255));
       // DrawCircleMidpoint(hdc, Xc, Yc, R, RGB(255, 255, 0));
        DrawCircleModifiedMidpoint(hdc, Xc, Yc, R, RGB(0, 255, 255));





        ReleaseDC(hwnd, hdc);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);

        // Redraw the line when window repaints
                      //// Lines /////

        //DrawLineDDA(hdc, X1, Y1, X2, Y2, RGB(255, 0, 0));
       // DrawLineMidpoint(hdc, X1, Y1, X2, Y2, RGB(0, 255, 0));
      //  DrawLineParametric(hdc, X1, Y1, X2, Y2, RGB(0, 0, 255));
     //   DrawLineDirect(hdc, X1, Y1, X2, Y2, RGB(0, 0, 0));

                //// Circles /////

      //  DrawCircleDirect(hdc, Xc, Yc, R, RGB(0, 0, 0));
      //  DrawCirclePolar(hdc, Xc, Yc, R, RGB(0, 0, 0));
       // DrawCircleIterativePolar(hdc, Xc, Yc, R, RGB(0, 0, 0));
      //  DrawCircleMidpoint(hdc, Xc, Yc, R, RGB(0, 0, 0));
        DrawCircleModifiedMidpoint(hdc, Xc, Yc, R, RGB(0, 0, 0));

 

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, m, wp, lp);
    }
    return 0;
}


int APIENTRY WinMain(HINSTANCE hi, HINSTANCE pi, LPSTR cmd, int nsh)
{
    WNDCLASS wc = {};
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.lpszClassName = L"MyClass";
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hi;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"MyClass", L"Welcome ^_^!", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 500, NULL, NULL, hi, 0);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}