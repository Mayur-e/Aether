#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>

// Boundary Flood Fill (4-connected) — standalone GLUT visualiser
// A triangle outline is drawn into a pixel buffer; flood fill fills the interior.

const int  W = 200, H = 200;         // virtual pixel grid
const int  HALF = 100;               // half-extent: maps to GLUT coords via gluOrtho2D

// Colour values (stored as int for easy comparison)
const int COL_BG       = 0;
const int COL_BOUNDARY = 1;
const int COL_FILL     = 2;

int pixelBuf[H][W];

// ---- Draw a triangle outline into the buffer using DDA ----
void setPixelBuf(int x, int y, int col)
{
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    pixelBuf[y][x] = col;
}

void ddaLine(int x1, int y1, int x2, int y2)
{
    int dx = x2-x1, dy = y2-y1;
    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) { setPixelBuf(x1, y1, COL_BOUNDARY); return; }
    float xi = (float)dx/steps, yi = (float)dy/steps;
    float x = (float)x1, y = (float)y1;
    for (int i = 0; i <= steps; ++i)
    {
        setPixelBuf((int)std::round(x), (int)std::round(y), COL_BOUNDARY);
        x += xi; y += yi;
    }
}

// ---- Iterative 4-connected boundary fill ----
void boundaryFill4(int x, int y)
{
    struct P { int x, y; };
    std::vector<P> stack;
    stack.reserve(W * H);
    stack.push_back({x, y});

    while (!stack.empty())
    {
        P cur = stack.back(); stack.pop_back();

        if (cur.x < 0 || cur.x >= W || cur.y < 0 || cur.y >= H) continue;
        int& c = pixelBuf[cur.y][cur.x];
        if (c == COL_BOUNDARY || c == COL_FILL) continue;

        c = COL_FILL;
        stack.push_back({cur.x+1, cur.y});
        stack.push_back({cur.x-1, cur.y});
        stack.push_back({cur.x,   cur.y+1});
        stack.push_back({cur.x,   cur.y-1});
    }
}

// ---- GLUT callbacks ----
void drawGrid()
{
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            if      (pixelBuf[y][x] == COL_BOUNDARY) glColor3f(1.0f, 1.0f, 0.2f);
            else if (pixelBuf[y][x] == COL_FILL)     glColor3f(0.9f, 0.35f, 0.15f);
            else continue;

            // Map buffer coords to world coords
            float wx = (float)(x - HALF);
            float wy = (float)(y - HALF);
            glVertex2f(wx, wy);
        }
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    glFlush();
}

void init()
{
    glClearColor(0.07f, 0.07f, 0.11f, 1.0f);
    gluOrtho2D(-HALF, HALF, -HALF, HALF);
}

int main(int argc, char** argv)
{
    std::memset(pixelBuf, COL_BG, sizeof(pixelBuf));

    // Draw triangle boundary (buffer coords centred at HALF,HALF)
    // Vertices: top(100,170), left(30,50), right(170,50)
    int tx=100,ty=170, lx=30,ly=50, rx=170,ry=50;
    ddaLine(tx,ty, lx,ly);
    ddaLine(lx,ly, rx,ry);
    ddaLine(rx,ry, tx,ty);

    // Seed inside the triangle
    int seedX = 100, seedY = 80;
    std::cout << "Boundary Flood Fill (4-connected)\n";
    std::cout << "Filling triangle. Seed: (" << seedX-HALF << ", " << seedY-HALF << ")\n\n";
    boundaryFill4(seedX, seedY);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(150, 80);
    glutCreateWindow("Boundary Flood Fill (4-Connected)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
