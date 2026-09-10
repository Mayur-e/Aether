#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// Input coordinates
int xStart, yStart, xEnd, yEnd;

// Store generated points
vector<pair<int, int>> points;

// Plot a single pixel
void setPixel(int x, int y)
{
    glBegin(GL_POINTS);
        glVertex2i(x, y);
    glEnd();
}

// Draw X and Y axes
void drawAxes()
{
    glColor3f(1.0, 1.0, 1.0);   // White

    glBegin(GL_LINES);

    // X-axis
    glVertex2i(-50, 0);
    glVertex2i(50, 0);

    // Y-axis
    glVertex2i(0, -50);
    glVertex2i(0, 50);

    glEnd();
}

// DDA Algorithm (Calculates points only)
void DDA()
{
    points.clear();

    int dx = xEnd - xStart;
    int dy = yEnd - yStart;

    int steps = max(abs(dx), abs(dy));

    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;

    float x = xStart;
    float y = yStart;

    for (int i = 0; i <= steps; i++)
    {
        points.push_back({(int)round(x), (int)round(y)});

        x += xInc;
        y += yInc;
    }
}

// Print generated points
void printPoints()
{
    cout << "\nPoints Generated using DDA Algorithm\n";

    for (auto p : points)
    {
        cout << "(" << p.first << ", " << p.second << ")" << endl;
    }
}

// Draw stored points
void drawLine()
{
    glColor3f(0.0, 1.0, 0.0);   // Green

    for (auto p : points)
    {
        setPixel(p.first, p.second);
    }
}

// Display callback
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawAxes();

    drawLine();

    glFlush();
}

// OpenGL Initialization
void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glPointSize(5.0);

    gluOrtho2D(-50, 50, -50, 50);
}

// Main Function
int main(int argc, char **argv)
{
    cout << "Enter Starting Point (x1 y1): ";
    cin >> xStart >> yStart;

    cout << "Enter Ending Point (x2 y2): ";
    cin >> xEnd >> yEnd;

    // Generate and print points once
    DDA();
    printPoints();

    // OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(700, 700);
    glutInitWindowPosition(200, 100);

    glutCreateWindow("DDA Line Drawing Algorithm");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
