// Project-Graphics.cpp : Defines the entry point for the application.

#include "Menu.h"
#include <algorithm>
#include <stack>
#include <vector>
#include <climits>
#include <cstring>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// --- Flood Fill Functions ---

void FloodFillRecursive(HDC hdc, int x, int y, COLORREF bc, COLORREF fc)
{
    COLORREF C = GetPixel(hdc, x, y);
    if (C == bc || C == fc) return;
    SetPixel(hdc, x, y, fc);
    FloodFillRecursive(hdc, x + 1, y, bc, fc);
    FloodFillRecursive(hdc, x - 1, y, bc, fc);
    FloodFillRecursive(hdc, x, y + 1, bc, fc);
    FloodFillRecursive(hdc, x, y - 1, bc, fc);
}

void FloodFillNonRecursive(HDC hdc, int x, int y, COLORREF bc, COLORREF fc)
{
    stack<POINT> st;
    st.push({ x, y });
    while (!st.empty())
    {
        POINT p = st.top();
        st.pop();
        COLORREF C = GetPixel(hdc, p.x, p.y);
        if (C == bc || C == fc) continue;
        SetPixel(hdc, p.x, p.y, fc);
        st.push({ p.x + 1, p.y });
        st.push({ p.x - 1, p.y });
        st.push({ p.x, p.y + 1 });
        st.push({ p.x, p.y - 1 });
    }
}

// --- Curve Fill Functions ---

typedef struct
{
    int xleft;
    int xright;
} EdgeTable[800];


void initializeEdgePoints(EdgeTable edgePoints)
{
    for (int i = 0; i < 800; i++)
    {
        edgePoints[i].xleft = INT_MAX;
        edgePoints[i].xright = INT_MIN;
    }
}

void EdgesToTable(Point& v1, Point& v2, EdgeTable& edgepoint)
{
    if (v1.Y == v2.Y) return; // horizontal edge, skip
    if (v1.Y > v2.Y) swap(v1, v2);

    int y = (int)ceil(v1.Y);
    double x = v1.X + (y - v1.Y) * (v2.X - v1.X) / (v2.Y - v1.Y);
    double m = (v2.X - v1.X) / (v2.Y - v1.Y);

    while (y < (int)v2.Y && y < 800)
    {
        if (x < edgepoint[y].xleft)
            edgepoint[y].xleft = (int)ceil(x);
        if (x > edgepoint[y].xright)
            edgepoint[y].xright = (int)floor(x);
        y++;
        x += m;
    }
}

void DrawPolygon(HDC hdc, vector<Point>& vertices, EdgeTable& edgetable)
{
    int n = (int)vertices.size();
    if (n < 2) return;

    Point v1 = vertices[n - 1];
    for (int i = 0; i < n; i++)
    {
        Point v2 = vertices[i];
        EdgesToTable(v1, v2, edgetable);
        v1 = v2;
    }
}


void Table2screen(HDC hdc, EdgeTable& edgetable, COLORREF c)
{
    for (int y = 0; y < 800; y++)
    {
        if (edgetable[y].xleft == INT_MAX || edgetable[y].xright == INT_MIN)
            continue;
        if (edgetable[y].xleft < edgetable[y].xright)
        {
            DrawLineMidpoint(hdc, edgetable[y].xleft, y, edgetable[y].xright, y, c);
        }
    }
}

void CurveFill(HDC hdc, vector<Point>& vertices, COLORREF c)
{
    EdgeTable edgetable;
    initializeEdgePoints(edgetable);
    DrawPolygon(hdc, vertices, edgetable);
    Table2screen(hdc, edgetable, c);
}

// --- Non-convex polygon filling (Scanline Fill Algorithm) ---

struct Node {
    double x;
    int ymax;
    double mi;  // inverse slope (1/m)
    Node* next;
};

constexpr int MAX_HEIGHT = 1000;
Node* Table[MAX_HEIGHT]; // Edge Table, indexed by scanline

void InsertNodeInSortedLinedList(Node*& head, Node* edge) {
    if (!head || edge->x < head->x) {
        edge->next = head;
        head = edge;
    }
    else {
        Node* current = head;
        while (current->next && current->next->x < edge->x)
            current = current->next;
        edge->next = current->next;
        current->next = edge;
    }
}

void InitEdgeTable(const vector<Point>& polygon) {
    memset(Table, 0, sizeof(Table));
    int n = (int)polygon.size();

    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n]; // wrap-around to first point

        if ((int)p1.Y == (int)p2.Y) continue; // horizontal edges skipped

        if (p1.Y > p2.Y) swap(p1, p2);

        Node* node = new Node;
        node->ymax = (int)p2.Y;
        node->x = p1.X;
        node->mi = (p2.X - p1.X) / (p2.Y - p1.Y);
        node->next = nullptr;

        int yIndex = (int)p1.Y;
        if (yIndex >= 0 && yIndex < MAX_HEIGHT)
            InsertNodeInSortedLinedList(Table[yIndex], node);
        else
            delete node; // out of bounds, avoid memory leak
    }
}

void CleanupEdgeTable() {
    for (int i = 0; i < MAX_HEIGHT; i++) {
        Node* current = Table[i];
        while (current) {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        Table[i] = nullptr;
    }
}

void FillPolygon(HDC hdc, COLORREF color, const vector<Point>& polygon) {
    InitEdgeTable(polygon);
    vector<Node*> ActiveEdgeTable;

    // Find first non-empty scanline
    int y = 0;
    while (y < MAX_HEIGHT && Table[y] == nullptr) y++;

    while (y < MAX_HEIGHT) {
        // Add edges from ET[y] to AET
        for (Node* cur = Table[y]; cur != nullptr; cur = cur->next) {
            ActiveEdgeTable.push_back(cur);
        }

        // Remove edges for which y == ymax
        ActiveEdgeTable.erase(remove_if(ActiveEdgeTable.begin(), ActiveEdgeTable.end(),
            [y](Node* e) { return y >= e->ymax; }),
            ActiveEdgeTable.end());

        // Sort by current x
        sort(ActiveEdgeTable.begin(), ActiveEdgeTable.end(),
            [](Node* a, Node* b) { return a->x < b->x; });

        for (size_t i = 0; i + 1 < ActiveEdgeTable.size(); i += 2) {
            int x1 = (int)ceil(ActiveEdgeTable[i]->x);
            int x2 = (int)floor(ActiveEdgeTable[i + 1]->x);
            for (int x = x1; x <= x2; x++) {
                SetPixel(hdc, x, y, color);
            }
        }

        for (auto* edge : ActiveEdgeTable) {
            edge->x += edge->mi;
        }

        y++;
    }

    CleanupEdgeTable(); // free allocated memory
}

// --- Hermite and Cardinal Spline Curves ---

void HermiteCurve(HDC hdc, Point p1, Point t1, Point p2, Point t2, COLORREF color) {
    int steps = 100; // Number of steps for the curve
    SetPixel(hdc, (int)p1.X, (int)p1.Y, color);
    for (int i = 1; i <= steps; i++) {
        double t = (double)i / steps;
        double t2_ = t * t;
        double t3 = t2_ * t;

        double h1 = 2 * t3 - 3 * t2_ + 1;
        double h2 = -2 * t3 + 3 * t2_;
        double h3 = t3 - 2 * t2_ + t;
        double h4 = t3 - t2_;

        double x = h1 * p1.X + h2 * p2.X + h3 * t1.X + h4 * t2.X;
        double y = h1 * p1.Y + h2 * p2.Y + h3 * t1.Y + h4 * t2.Y;
        Point pt(x, y);

        SetPixel(hdc, (int)pt.X, (int)pt.Y, color);
    }
}

void CardinalSplineCurve(HDC hdc, vector<Point>& points, double tensionfactor, COLORREF color)
{
    int n = (int)points.size();
    if (n < 2) return;

    vector<Point> tangents(n);

    for (int i = 0; i < n; i++)
    {
        if (i == 0) {
            tangents[i] = (points[1] - points[0]) * tensionfactor;
        }
        else if (i == n - 1) {
            tangents[i] = (points[n - 1] - points[n - 2]) * tensionfactor;
        }
        else {
            tangents[i] = (points[i + 1] - points[i - 1]) * tensionfactor;
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        HermiteCurve(hdc, points[i], tangents[i], points[i + 1], tangents[i + 1], color);
    }
}

