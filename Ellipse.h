#include <windows.h>
#include <math.h>


// Author : Malak Hisham
// to run thr code use following commands 
// 

double pi = 4 * atan(1.0);

void Draw4Pixel(HDC hdc, int x, int y, int Xc, int Yc, COLORREF c) {
    SetPixel(hdc, Xc + x, Yc + y, c);
    SetPixel(hdc, Xc - x, Yc + y, c);
    SetPixel(hdc, Xc + x, Yc - y, c);
    SetPixel(hdc, Xc - x, Yc - y, c);
}

void DirectEllipse(HDC hdc, int a, int b, int Xc, int Yc, COLORREF c) {
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;
    for (int x = 0; x <= a; x++) {
        long long y = (long long)b * sqrt(1.0 - (double)(x * x) / a2);
        Draw4Pixel(hdc, x, (int)y, Xc, Yc, c);
    }
}

void PolarEllipse(HDC hdc, int a, int b, int Xc, int Yc, COLORREF c) {
    for (double theta = 0; theta <= pi / 2; theta += 0.01) {
        int x = (int)(a * cos(theta));
        int y = (int)(b * sin(theta));
        Draw4Pixel(hdc, x, y, Xc, Yc, c);
    }
}

void MidPointEllipse(HDC hdc, int a, int b, int Xc, int Yc, COLORREF c) {
    int x = 0, y = b;
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;
    long long two_a2 = 2 * a2;
    long long two_b2 = 2 * b2;

    // Region 1: Slope < 1
    long long d = b2 - a2 * b + a2 / 4;
    while (b2 * x <= a2 * y) { // b^2 * x <= a^2 * y
        Draw4Pixel(hdc, x, y, Xc, Yc, c);
        x++;
        if (d < 0) {
            d += two_b2 * x + b2;
        }
        else {
            y--;
            d += two_b2 * x + b2 - two_a2 * y;
        }
    }

    // Region 2: Slope >= 1
    d = b2 * (x * x + x) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        Draw4Pixel(hdc, x, y, Xc, Yc, c);
        y--;
        if (d < 0) {
            x++;
            d += two_b2 * x - two_a2 * y + a2;
        }
        else {
            d += -two_a2 * y + a2;
        }
    }
}



