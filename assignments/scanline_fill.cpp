#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// Scanline Polygon Fill — standalone GLUT visualiser
// Demonstrates the scanline fill algorithm on a concave polygon.

struct Point2i { int x, y; };

// Polygon vertices (concave L-shape)
std::vector<Point2i> polygon = {
    {-30,  -30},
    { 30,  -30},
    { 30,    0},
    {  0,    0},
    {  0,   30},
    {-30,   30}
};

std::vector<Point2i> filledPixels;

void scanlineFill()
{
    filledPixels.clear();
    int n = (int)polygon.size();

    int yMin = polygon[0].y, yMax = polygon[0].y;
    for (const auto& p : polygon)
    {
        yMin = std::min(yMin, p.y);
        yMax = std::max(yMax, p.y);
    }

    for (int y = yMin; y <= yMax; ++y)
    {
        std::vector<float> xs;

        for (int i = 0; i < n; ++i)
        {
            const Point2i& p1 = polygon[(size_t)i];
            const Point2i& p2 = polygon[(size_t)((i+1) % n)];

            if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y))
            {
                float x = p1.x + (float)(y - p1.y) / (float)(p2.y - p1.y)
                               * (float)(p2.x - p1.x);
                xs.push_back(x);
            }
        }

        std::sort(xs.begin(), xs.end());

        for (size_t k = 0; k + 1 < xs.size(); k += 2)
        {
            int xStart = (int)std::ceil(xs[k]);
            int xEnd   = (int)std::floor(xs[k+1]);
            for (int x = xStart; x <= xEnd; ++x)
                filledPixels.push_back({x, y});
        }
    }

    std::cout << "Total filled pixels: " << filledPixels.size() << "\n";
}

void drawAxes()
{
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
      glVertex2i(-50,0); glVertex2i(50,0);
      glVertex2i(0,-50); glVertex2i(0,50);
    glEnd();
}

void drawPolygonOutline()
{
    glColor3f(1.0f, 1.0f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : polygon)
        glVertex2i(p.x, p.y);
    glEnd();
    glLineWidth(1.0f);
}

void drawFill()
{
    glColor3f(0.2f, 0.7f, 1.0f);
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (const auto& p : filledPixels)
        glVertex2i(p.x, p.y);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawFill();
    drawPolygonOutline();
    glFlush();
}

void init()
{
    glClearColor(0.06f, 0.06f, 0.10f, 1.0f);
    glPointSize(1.0f);
    gluOrtho2D(-50, 50, -50, 50);
}

int main(int argc, char** argv)
{
    std::cout << "Scanline Polygon Fill Algorithm\n";
    std::cout << "Filling an L-shaped concave polygon.\n\n";

    scanlineFill();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(150, 80);
    glutCreateWindow("Scanline Polygon Fill");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
