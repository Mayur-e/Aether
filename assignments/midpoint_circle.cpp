#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

// Circle center and radius
int xc, yc, radius;

// Store generated circle points
vector<pair<int, int>> points;

// Plot a single pixel
void setPixel(int x, int y)
{
    glBegin(GL_POINTS);
        glVertex2i(x, y);
    glEnd();
}

// Add the 8 symmetric points
void addSymmetricPoints(int x, int y)
{
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc + x, yc - y});
    points.push_back({xc - x, yc - y});

    points.push_back({xc + y, yc + x});
    points.push_back({xc - y, yc + x});
    points.push_back({xc + y, yc - x});
    points.push_back({xc - y, yc - x});
}

// Midpoint Circle Drawing Algorithm
void MidpointCircle()
{
    points.clear();

    int x = 0;
    int y = radius;

    // Initial decision parameter
    int p = 1 - radius;

    while (x <= y)
    {
        // Generate all 8 symmetric points
        addSymmetricPoints(x, y);

        if (p < 0)
        {
            // Choose East pixel
            p = p + 2 * x + 3;
        }
        else
        {
            // Choose South-East pixel
            p = p + 2 * (x - y) + 5;
            y--;
        }

        x++;
    }
}

// Print generated points
void printPoints()
{
    cout << "\nPoints Generated using Midpoint Circle Algorithm\n";
    cout << "------------------------------------------------\n";

    for (auto p : points)
    {
        cout << "(" << p.first << ", " << p.second << ")" << endl;
    }

    cout << "\nTotal points generated: " << points.size() << endl;
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

// Draw circle
void drawCircle()
{
    glColor3f(0.0, 1.0, 0.0);

    for (auto p : points)
    {
        setPixel(p.first, p.second);
    }
}

// Draw center point
void drawCenter()
{
    glColor3f(1.0, 0.0, 0.0);

    glPointSize(8.0);

    setPixel(xc, yc);

    glPointSize(5.0);
}

// Display function
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawAxes();
    drawCircle();
    drawCenter();

    glFlush();
}

// OpenGL initialization
void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glPointSize(5.0);

    gluOrtho2D(-50, 50, -50, 50);
}

// Main function
int main(int argc, char **argv)
{
    cout << "Enter Center of Circle (xc yc): ";
    cin >> xc >> yc;

    cout << "Enter Radius: ";
    cin >> radius;

    // Generate points once
    MidpointCircle();

    // Print points once
    printPoints();

    // Initialize OpenGL
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(700, 700);

    glutInitWindowPosition(200, 100);

    glutCreateWindow("Midpoint Circle Drawing Algorithm");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
