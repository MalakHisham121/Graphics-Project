#include <windows.h>
#include "Menu.h"
#include <iostream>
#include <vector>
#include <math.h>
// Author: Malak Hisham
// Use following commands to compile and run the code
// g++ -o clipping clipping.cpp -mwindows
// ./clipping
using namespace std;

struct Linepoint {
    double x, y;
    Linepoint() : x(0), y(0) {}
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

struct Poly {
    vector<Linepoint> vertices;
    vector<code> verticesPos;
};

struct Line {
    Linepoint p1, p2;
};

struct Circle {
    double xc, yc, r;
};

code SetCode(Linepoint& p, Rectangl& R) {
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

void LineClipping(Rectangl& R, Linepoint& p1, Linepoint& p2, code& code1, code& code2) {
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

vector<Linepoint> check(int ref, Linepoint& p1, Linepoint& p2, bool greaterisInner, bool X) {
    vector<Linepoint> result;
    double slope = (p2.x != p1.x) ? (p2.y - p1.y) / (p2.x - p1.x) : 0;
    bool isVertical = (p2.x == p1.x);
    if (greaterisInner) {
        if (X) {
            if (p1.x >= ref && p2.x >= ref) {
                result.push_back(p2);
            }
            else if (p1.x >= ref && p2.x < ref) {
                double x = ref;
                double y = p1.y + slope * (ref - p1.x);
                result.push_back(Linepoint(x, y));
            }
            else if (p1.x < ref && p2.x >= ref) {
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
    }
    else {
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

void RectangleClipping(HDC hdc, int type, Rectangl R, Linepoint p1, Linepoint p2, Poly poly) {
    if (type == 0) {
        // Drawing a single point for point clipping
        // Could call a custom point-drawing function here instead of SetPixel
        code c = SetCode(p1, R);
        if (!c.whole) {
            SetPixel(hdc, round(p1.x), round(p1.y), RGB(0, 0, 255));
        }
    }
    else if (type == 1) {
        code code1 = SetCode(p1, R);
        code code2 = SetCode(p2, R);

        LineClipping(R, p1, p2, code1, code2);
        if (!(code1.whole || code2.whole)) {
            DrawLineDDA(hdc, p1.x, p1.y, p2.x, p2.y, RGB(0, 0, 255));
        }
    }
    else if (type == 2) {
        vector<Linepoint> polygon = poly.vertices;
        if (polygon.empty() || polygon.size() < 3) return;
        // Drawing the original polygon in red
        // Could call a custom polygon-drawing function or loop over a line-drawing function (e.g., Bresenham's) for each edge
        for (size_t i = 1; i < polygon.size(); ++i) {
            DrawLineDDA(hdc, round(polygon[i - 1].x), round(polygon[i - 1].y), round(polygon[i].x), round(polygon[i].y), RGB(255, 0, 0));

        }
        DrawLineDDA(hdc, round(polygon.back().x), round(polygon.back().y), round(polygon[0].x), round(polygon[0].y), RGB(255, 0, 0));
        vector<Linepoint> input = polygon;
        vector<Linepoint> output;
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
        // Drawing the clipped polygon in blue

        if (!output.empty()) {
            for (size_t i = 1; i < output.size(); ++i) {
                DrawLineDDA(hdc, round(output[i - 1].x), round(output[i - 1].y), round(output[i].x), round(output[i].y), RGB(0, 0, 255));
            }
            DrawLineDDA(hdc, round(output.back().x), round(output.back().y), round(output[0].x), round(output[0].y), RGB(0, 0, 255));
        }
    }
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


void CircleClipping(HDC hdc, int type, Circle C, Linepoint& p1, Linepoint& p2) {
    int d1 = (p1.x - C.xc) * (p1.x - C.xc) + (p1.y - C.yc) * (p1.y - C.yc);
    int d2 = (p2.x - C.xc) * (p2.x - C.xc) + (p2.y - C.yc) * (p2.y - C.yc);
    int rsqured = C.r * C.r;
    bool draw = 0;
    if (d1 <= rsqured && d2 <= rsqured) {
        // Drawing a point or line fully inside the circle
        // Could call a custom point or line-drawing function here
        draw = 1;
    }
    else if (d1 <= rsqured) {
        auto intersections = findIntersections(p1, p2, C);
        if (!intersections.empty()) {
            p2 = intersections[0];
            draw = 1;
        }
    }
    else if (d2 <= rsqured) {
        auto intersections = findIntersections(p1, p2, C);
        if (!intersections.empty()) {
            p1 = intersections[0];
            draw = 1;
        }
    }
    else {
        auto intersections = findIntersections(p1, p2, C);
        if (intersections.size() == 2) {
            p1 = intersections[0];
            p2 = intersections[1];
            draw = 1;
        }
    }
    // Drawing the clipping circle
    // Could call a custom circle-drawing function (e.g., Midpoint circle algorithm) here

    if (draw) {

        DrawLineDDA(hdc, p1.x, p1.y, p2.x, p2.y, RGB(0, 0, 255));

    }
}


enum ClippingWindow { NONE, RECTANGLE, SQUARE, CIRCLE };
enum ObjectType { OBJ_NONE, OBJ_POINT, LINE, POLYGON };
enum InputState { IDLE, SET_CLIP_WINDOW_P1, SET_CLIP_WINDOW_P2, SET_POINT, SET_LINE_P1, SET_LINE_P2, SET_POLYGON };

ClippingWindow currentWindow = NONE;
ObjectType currentObject = OBJ_NONE;
InputState currentState = IDLE;
Rectangl clipRect;
Circle clipCircle;
Linepoint point, lineP1, lineP2;
Poly polygon;
vector<Linepoint> tempPolygonVertices;
bool clipWindowDefined = false;

void DrawClippingWindow(HDC hdc) {


    if (currentWindow == RECTANGLE) {
        DrawRectangle(hdc, clipRect.X_min, clipRect.Y_min, clipRect.X_max - clipRect.X_min, clipRect.Y_max - clipRect.Y_min, RGB(0, 0, 0));
    }
    else if (currentWindow == SQUARE) {
        DrawSquare(hdc, clipRect.X_min, clipRect.Y_min, clipRect.Y_max - clipRect.Y_min, RGB(0, 0, 0));

    }
    else if (currentWindow == CIRCLE) {
        DrawCircleMidpoint(hdc, clipCircle.xc, clipCircle.yc, clipCircle.r, RGB(0, 0, 0));
    }

}