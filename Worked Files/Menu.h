#pragma once

//#include "resource.h"

// Name : Shahd Elnassag
// ID : 20220533

#include <windows.h>
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
    int dx = X2 - X1;
    int dy = Y2 - Y1;

    int steps = max(abs(dx), abs(dy));

    double xIncrement = dx / (double)steps;
    double yIncrement = dy / (double)steps;

    double x = X1;
    double y = Y1;

    for (int i = 0; i <= steps; i++) {
        SetPixel(hdc, Round(x), Round(y), c);
        x += xIncrement;
        y += yIncrement;
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
    double step = 1.0 / max(abs(dx), abs(dy));
    double t;
    for (t = 0.0; t <= 1.0; t += step) {
        int x = dx * t + X1;
        int y = dy * t + Y1;
        SetPixel(hdc, Round(x), Round(y), c);
    }

}

// 4. Direct Method
void DrawLineDirect(HDC hdc, int X1, int Y1, int X2, int Y2, COLORREF c) {

    int dx = X2 - X1; int dy = Y2 - Y1;
    if (abs(dx) >= abs(dy)) {
        double slope = (double)dy / dx;
        int X = X1;
        SetPixel(hdc, X1, Y1, c);
        while (X < X2) {
            X++;
            int Y = Round(slope * (X - X1) + Y1);
            SetPixel(hdc, X, Y, c);

        }
    }
    else {
        double slopeInverse = (double)dx / dy;
        int Y = Y1;
        SetPixel(hdc, X1, Y1, c);
        while (Y < Y2) {
            Y++;
            int X = Round(slopeInverse * (Y - Y1) + X1);
            SetPixel(hdc, X, Y, c);

        }

    }
}

//****************************************************************************************************************//

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

//****************************************************************************************************************//

//11. Filling Circle with lines after taking filling quarter from user
void FillingCircleWithLines(HDC hdc, int xc, int yc, int R, int quarter, int step, COLORREF color) {
    for (int y = -R; y <= R; y += step) {
        for (int x = -R; x <= R; x += step) {
            // (X*X) + (Y*Y) = R*R
            int dx = x;
            int dy = y;
            // draw in boundries
            if (dx * dx + dy * dy <= R * R) {
                bool drawLines = false;
                // Positive y goes downward.
                switch (quarter) {
                    // quarter 1
                case 1:
                    if (dx >= 0 && dy <= 0)
                        drawLines = true;
                    break;
                    // quarter 2
                case 2:
                    if (dx <= 0 && dy <= 0)
                        drawLines = true;
                    break;
                    // quarter 3
                case 3:
                    if (dx <= 0 && dy >= 0)
                        drawLines = true;
                    break;
                    // quarter 4 
                case 4:
                    if (dx >= 0 && dy >= 0)
                        drawLines = true;
                    break;

                }

                if (drawLines) {
                    // Calculate how far we can draw in x-direction to the circle edge
                    int Xdistance = (int)sqrt(R * R - dy * dy);

                    int x1 = xc, x2 = xc;

                    if (quarter == 1 || quarter == 4) {
                        x1 = xc;
                        x2 = xc + Xdistance;
                    }
                    else if (quarter == 2 || quarter == 3) {
                        x1 = xc - Xdistance;
                        x2 = xc;

                    }

                    DrawLineMidpoint(hdc, x1, yc + dy, x2, yc + dy, color);
                    break;
                }
            }
        }
    }
}

//****************************************************************************************************************//

//12. Filling Circle with other circles after taking filling quarter from user
void FillingCircleWithCircles(HDC hdc, int xc, int yc, int R, int quarter, int inraduis, COLORREF color) {
    for (int y = -R; y <= R; y += 2 * inraduis) {
        for (int x = -R; x <= R; x += 2 * inraduis) {
            int dx = xc + x;
            int dy = yc + y;
            // Ensure the small circle center is inside the big circle
            if ((x * x + y * y <= R * R)) {
                bool drawCircles = false;

                switch (quarter) {
                    // quarter 1
                case 1:
                    if (x >= 0 && y <= 0)
                        drawCircles = true;
                    break;
                    //   quarter 2
                case 2:
                    if (x <= 0 && y <= 0)
                        drawCircles = true;
                    break;
                    // quarter 3
                case 3:
                    if (x <= 0 && y >= 0)
                        drawCircles = true;
                    break;
                    // quarter 4
                case 4:
                    if (x >= 0 && y >= 0)
                        drawCircles = true;
                    break;

                }

                if (drawCircles) {
                    DrawCircleModifiedMidpoint(hdc, dx, dy, inraduis, color);
                }
            }
        }
    }
}

//****************************************************************************************************************//

//13. Filling Square with Hermit Curve[Vertical]
// Hermit Curve 
void DrawHermitCurve(HDC hdc, int X1, int Y1, int U1, int V1, int X2, int Y2, int U2, int V2, COLORREF C1, COLORREF C2, int minX, int minY, int maxX, int maxY)
{
    int alpha1 = 2 * X1 + U1 - 2 * X2 + U2;
    int beta1 = -3 * X1 - 2 * U1 + 3 * X2 - U2;
    int gama1 = U1;
    int delta1 = X1;


    int alpha2 = 2 * Y1 + V1 - 2 * Y2 + V2;
    int beta2 = -3 * Y1 - 2 * V1 + 3 * Y2 - V2;
    int gama2 = V1;
    int delta2 = Y1;

    // extract colore values
    //Red Color
    int r1 = GetRValue(C1);
    int r2 = GetRValue(C2);
    // Green Color
    int g1 = GetGValue(C1);
    int g2 = GetGValue(C2);
    // Blue Color
    int b1 = GetBValue(C1);
    int b2 GetBValue(C2);

    int alphaR = r2 - r1;
    int alphaG = g2 - g1;
    int alphaB = b2 - b1;

    double numberOfPoints = 100.0;
    double step = 1.0 / numberOfPoints;

    for (double t = 0; t <= 1; t += step) {

        int X = alpha1 * (t * t * t) + beta1 * (t * t) + gama1 * t + delta1;
        int Y = alpha2 * (t * t * t) + beta2 * (t * t) + gama2 * t + delta2;



        if (X >= minX && X <= maxX && Y >= minY && Y <= maxY) {
            int R = Round(alphaR * t + r1);
            int G = Round(alphaG * t + g1);
            int B = Round(alphaB * t + b1);
            SetPixel(hdc, X, Y, RGB(R, G, B));
        }


    }

}

void DrawSquare(HDC hdc, int x1, int y1, int length, COLORREF c) {
    int x2 = x1 + length;
    int y2 = y1 + length;

    // Draw four sides
    DrawLineDDA(hdc, x1, y1, x2, y1, c); // Top
    DrawLineDDA(hdc, x2, y1, x2, y2, c); // Right
    DrawLineDDA(hdc, x2, y2, x1, y2, c); // Bottom
    DrawLineDDA(hdc, x1, y2, x1, y1, c); // Left
}

void FillingSquareWithHermiteCurves(HDC hdc, int x1, int y1, int length, int step, COLORREF bc, COLORREF fc) {
    int x2 = x1 + length;
    int y2 = y1 + length;

    for (int x = x1; x <= x2; x += step) {
        // Hermite curve Vertical line from (x, y1) to (x, y2)
        int Y1 = y1, X1 = x;
        int Y2 = y2, X2 = x;

        // Tangents in x direction (Vertical), so vertical component = 0
        int U1 = 80, V1 = length / 2;
        int U2 = 150 / 2, V2 = -length / 2;

        DrawHermitCurve(hdc, X1, Y1, U1, V1, X2, Y2, U2, V2, bc, fc, x1, y1, x2, y2);
    }
}

//****************************************************************************************************************//

//14. Filling Rectangle with Bezier Curve[horizontal]
// Bezier Curve 

struct Point
{
    double X, Y;
    Point() : X(0), Y(0) {}
    Point(double x, double y) : X(x), Y(y) {}

    Point operator+(const Point& other) const {
        return Point(X + other.X, Y + other.Y);
    }
    Point operator-(const Point& other) const {
        return Point(X - other.X, Y - other.Y);
    }
    Point operator*(double scalar) const {
        return Point(X * scalar, Y * scalar);
    }

};

void BezierCurveInterpolated(HDC hdc, Point P1, COLORREF C1, Point P2, COLORREF C2, Point P3, COLORREF C3, Point P4, COLORREF C4, int xmin, int ymin, int xmax, int ymax)
{
    double alpha1 = -P1.X + 3 * P2.X - 3 * P3.X + P4.X;
    double beta1 = 3 * P1.X - 6 * P2.X + 3 * P3.X;
    double gamma1 = -3 * P1.X + 3 * P2.X;
    double sigma1 = P1.X;

    double alpha2 = -P1.Y + 3 * P2.Y - 3 * P3.Y + P4.Y;
    double beta2 = 3 * P1.Y - 6 * P2.Y + 3 * P3.Y;
    double gamma2 = -3 * P1.Y + 3 * P2.Y;
    double sigma2 = P1.Y;

    //  Red
    double R1 = GetRValue(C1), R2 = GetRValue(C2), R3 = GetRValue(C3), R4 = GetRValue(C4);
    double alphaR = -R1 + 3 * R2 - 3 * R3 + R4;
    double betaR = 3 * R1 - 6 * R2 + 3 * R3;
    double gammaR = -3 * R1 + 3 * R2;
    double sigmaR = R1;

    // Green
    double G1 = GetGValue(C1), G2 = GetGValue(C2), G3 = GetGValue(C3), G4 = GetGValue(C4);
    double alphaG = -G1 + 3 * G2 - 3 * G3 + G4;
    double betaG = 3 * G1 - 6 * G2 + 3 * G3;
    double gammaG = -3 * G1 + 3 * G2;
    double sigmaG = G1;

    // Blue
    double B1 = GetBValue(C1), B2 = GetBValue(C2), B3 = GetBValue(C3), B4 = GetBValue(C4);
    double alphaB = -B1 + 3 * B2 - 3 * B3 + B4;
    double betaB = 3 * B1 - 6 * B2 + 3 * B3;
    double gammaB = -3 * B1 + 3 * B2;
    double sigmaB = B1;

    for (double t = 0; t <= 1; t += 0.01)
    {
        int X = Round(alpha1 * (t * t * t) + beta1 * (t * t) + gamma1 * t + sigma1);
        int Y = Round(alpha2 * (t * t * t) + beta2 * (t * t) + gamma2 * t + sigma2);

        // Check boundaries before drawing pixel
        if (X >= xmin && X <= xmax && Y >= ymin && Y <= ymax) {
            int R = Round(alphaR * t * t * t + betaR * t * t + gammaR * t + sigmaR);
            int G = Round(alphaG * t * t * t + betaG * t * t + gammaG * t + sigmaG);
            int B = Round(alphaB * t * t * t + betaB * t * t + gammaB * t + sigmaB);

            SetPixel(hdc, X, Y, RGB(R, G, B));
        }
    }
}

void DrawRectangle(HDC hdc, int x1, int y1, int length, int width, COLORREF c) {
    int x2 = x1 + length;
    int y2 = y1 + width;

    // Draw four sides
    DrawLineParametric(hdc, x1, y1, x2, y1, c); // Top
    DrawLineParametric(hdc, x2, y1, x2, y2, c); // Right
    DrawLineParametric(hdc, x2, y2, x1, y2, c); // Bottom
    DrawLineParametric(hdc, x1, y2, x1, y1, c); // Left
}

COLORREF InterpolateColor(COLORREF c1, COLORREF c2, double t) {
    int R = (int)((1 - t) * GetRValue(c1) + t * GetRValue(c2));
    int G = (int)((1 - t) * GetGValue(c1) + t * GetGValue(c2));
    int B = (int)((1 - t) * GetBValue(c1) + t * GetBValue(c2));
    return RGB(R, G, B);
}

void FillingRectangleWithBezierCurves(HDC hdc, int x1, int y1, int length, int width, int step, COLORREF bc, COLORREF fc) {
    int x2 = x1 + length;
    int y2 = y1 + width;

    // Control points for Bezier Horizontal line at each x coordinate
    for (int y = y1; y <= y2; y += step) {
        // Horizontal Bezier from left (x1) to right (x2) at vertical position y
        double curve = width * 0.3;
        Point P1(x1, y);
        Point P2(x1 + length / 4.0, y - curve);
        Point P3(x1 + 3 * length / 4.0, y + curve);
        Point P4(x2, y);

        double t = double(y - y1) / width;

        COLORREF C1 = InterpolateColor(bc, fc, t);
        COLORREF C2 = InterpolateColor(bc, fc, t);
        COLORREF C3 = InterpolateColor(bc, fc, t);
        COLORREF C4 = InterpolateColor(bc, fc, t);

        BezierCurveInterpolated(hdc, P1, C1, P2, C2, P3, C3, P4, C4, x1, y1, x2, y2);
    }
}

//****************************************************************************************************************//

 //15. Convex and Non-Convex Filling Algorithm
// Convex Polygon algorithm 
// Data Structure
//#define MAXENTRIES 800
//struct edgeTable {
//
//    int xLeft;
//    int xRight;
//};
//// Use Struct Point from above ^_^
//// Utaliit Functions
////1.Initialize
//void InitalizeTable(edgeTable tbl[]) {
//    for (int i = 0; i < MAXENTRIES; i++) {
//        tbl[i].xLeft = 100000;
//        tbl[i].xRight = -100000;
//    }
//}
////2. Evalate edge
//void ScanEdge(Point P1, Point P2, edgeTable tbl[]) {
//    if (P1.Y == P2.Y) return;
//    if (P1.Y > P2.Y) swap(P1, P2);
//    int y = P1.Y; int x = P1.X;
//    double inversSlope = (P2.X - P1.X) / (P2.Y - P1.Y);
//    while (y < P2.Y) {
//        if (x < tbl[y].xLeft) {
//            tbl[y].xLeft = (int)ceil(x);
//        }
//        if (x > tbl[y].xRight) {
//            tbl[y].xRight = (int)floor(x);
//        }
//        y++;
//        x += inversSlope;
//    }
//}
////3.raw scan lines stored in the table
//void Ploygon2Table(Point P[], int n, edgeTable tbl[]) {
//    Point P1 = P[n - 1];
//    for (int i = 0; i < n; i++)
//    {
//        Point P2 = P[i];
//        ScanEdge(P1, P2, tbl);
//        P1 = P[i];
//    }
//
//}
////4.draw scan lines to screen
//
//void DrawPloygonLine(HDC hdc, edgeTable tbl[], COLORREF c) {
//    for (int y = 0; y < MAXENTRIES; y++) {
//        if (tbl[y].xLeft < tbl[y].xRight) {
//            // Draw a visible horizontal line on this scanline
//            DrawLineMidpoint(hdc, tbl[y].xLeft, y, tbl[y].xRight, y, c);
//        }
//    }
//}
//
////5. main function of the algorithm
//void ConvexFill(HDC hdc, Point P[], int n, COLORREF c) {
//    edgeTable tbl[MAXENTRIES];
//    InitalizeTable(tbl);
//    Ploygon2Table(P, n, tbl);
//    DrawPloygonLine(hdc, tbl, c);
//
//}

//****************************************************************************************************************//

// 16. Recursive and Non-Recursive Flood Fill
//1.Recursive Flood Fill
//void FloodFill(HDC hdc, int x, int y, COLORREF bc, COLORREF fc) {
//    COLORREF c = GetPixel(hdc, x, y);
//    if (c == bc || c == fc) {
//        return;
//    }
//    SetPixel(hdc, x, y, fc);
//
//    FloodFill(hdc, x + 1, y, bc, fc);
//    FloodFill(hdc, x - 1, y, bc, fc);
//    FloodFill(hdc, x, y - 1, bc, fc);
//    FloodFill(hdc, x, y + 1, bc, fc);
//
//
//}

// Global variables
//int X1 = 0, Y1 = 0, X2 = 0, Y2 = 0;
//int U1 = 100, V1 = -200, U2 = 100, V2 = 200;
// Global variables to store line endpoints
int extern X1, Y1, X2, Y2;
// Global variables to Circle
int extern Xc, Yc, R;
// Get quareter
//static int selectQuarter = 1; // default quarter

