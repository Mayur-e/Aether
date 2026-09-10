#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

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
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_LINES);

    // X-axis
    glVertex2i(-50, 0);
    glVertex2i(50, 0);

    // Y-axis
    glVertex2i(0, -50);
    glVertex2i(0, 50);

    glEnd();
}

// Bresenham Algorithm
void Bresenham()
{
    points.clear();

    int dx = xEnd - xStart;
    int dy = yEnd - yStart;

    int p = 2 * dy - dx;

    int x = xStart;
    int y = yStart;

    points.push_back({x, y});

    while (x < xEnd)
    {
        if (p < 0)
        {
            x++;
            p = p + 2 * dy;
        }
        else
        {
            x++;
            y++;
            p = p + 2 * dy - 2 * dx;
        }

        points.push_back({x, y});
    }
}

// Print generated points
void printPoints()
{
    cout << "\nPoints Generated using Bresenham Algorithm\n";

    for (auto p : points)
    {
        cout << "(" << p.first << ", " << p.second << ")" << endl;
    }
}

// Draw stored points
void drawLine()
{
    glColor3f(0.0, 1.0, 0.0);

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

// Initialization
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

    // Check slope
    if ((xEnd - xStart) < (yEnd - yStart))
    {
        cout << "\nThis implementation supports only slope between 0 and 1.\n";
        return 0;
    }

    Bresenham();

    printPoints();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(700, 700);

    glutInitWindowPosition(200, 100);

    glutCreateWindow("Bresenham Line Drawing Algorithm");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
