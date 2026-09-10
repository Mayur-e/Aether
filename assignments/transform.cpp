#include <GL/glut.h>
#include <iostream>
#include <cmath>
using namespace std;

struct Point {
    float x, y;
};

Point original[20], transformed[20];
int n;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Original polygon
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++)
        glVertex2f(original[i].x, original[i].y);
    glEnd();

    // Transformed polygon
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++)
        glVertex2f(transformed[i].x, transformed[i].y);
    glEnd();

    glFlush();
}

// 1. Translation
void translation()
{
    float tx, ty;

    cout << "\nEnter Tx and Ty: ";
    cin >> tx >> ty;

    cout << "\nTranslation Matrix:\n";
    cout << "[ 1  0  0 ]\n";
    cout << "[ 0  1  0 ]\n";
    cout << "[ " << tx << "  " << ty << "  1 ]\n";

    for (int i = 0; i < n; i++) {
        transformed[i].x = original[i].x + tx;
        transformed[i].y = original[i].y + ty;
    }
}

// 2. Scaling
void scaling()
{
    float sx, sy;

    cout << "\nEnter Sx and Sy: ";
    cin >> sx >> sy;

    cout << "\nScaling Matrix:\n";
    cout << "[ " << sx << "  0  0 ]\n";
    cout << "[ 0  " << sy << "  0 ]\n";
    cout << "[ 0  0  1 ]\n";

    for (int i = 0; i < n; i++) {
        transformed[i].x = original[i].x * sx;
        transformed[i].y = original[i].y * sy;
    }
}

// 3. Rotation
void rotation()
{
    float angle;

    cout << "\nEnter angle in degrees: ";
    cin >> angle;

    float rad = angle * M_PI / 180.0;

    cout << "\nRotation Matrix:\n";
    cout << "[ cosA   sinA   0 ]\n";
    cout << "[ -sinA  cosA   0 ]\n";
    cout << "[ 0       0     1 ]\n";

    for (int i = 0; i < n; i++) {
        transformed[i].x =
            original[i].x * cos(rad) -
            original[i].y * sin(rad);

        transformed[i].y =
            original[i].x * sin(rad) +
            original[i].y * cos(rad);
    }
}

// 4. X-Shear
void xShear()
{
    float shx;

    cout << "\nEnter Shx: ";
    cin >> shx;

    cout << "\nX-Shear Matrix:\n";
    cout << "[ 1   0   0 ]\n";
    cout << "[ " << shx << "   1   0 ]\n";
    cout << "[ 0   0   1 ]\n";

    for (int i = 0; i < n; i++) {
        transformed[i].x =
            original[i].x + shx * original[i].y;

        transformed[i].y =
            original[i].y;
    }
}

// 5. Y-Shear
void yShear()
{
    float shy;

    cout << "\nEnter Shy: ";
    cin >> shy;

    cout << "\nY-Shear Matrix:\n";
    cout << "[ 1   " << shy << "   0 ]\n";
    cout << "[ 0    1      0 ]\n";
    cout << "[ 0    0      1 ]\n";

    for (int i = 0; i < n; i++) {
        transformed[i].x =
            original[i].x;

        transformed[i].y =
            original[i].y + shy * original[i].x;
    }
}

// 6. Composite Transformation
void composite()
{
    float tx, ty, sx, sy, angle;

    cout << "\nEnter Tx and Ty: ";
    cin >> tx >> ty;

    cout << "Enter Sx and Sy: ";
    cin >> sx >> sy;

    cout << "Enter rotation angle: ";
    cin >> angle;

    float rad = angle * M_PI / 180.0;

    cout << "\nComposite Transformation:\n";
    cout << "Translation -> Rotation -> Scaling\n";

    for (int i = 0; i < n; i++) {

        // Translation
        float x = original[i].x + tx;
        float y = original[i].y + ty;

        // Rotation
        float xr = x * cos(rad) - y * sin(rad);
        float yr = x * sin(rad) + y * cos(rad);

        // Scaling
        transformed[i].x = xr * sx;
        transformed[i].y = yr * sy;
    }
}

void printPoints()
{
    cout << "\nOriginal Points:\n";

    cout << "[\n";
    for (int i = 0; i < n; i++)
        cout << "  " << original[i].x
             << "  " << original[i].y
             << "  1\n";
    cout << "]\n";

    cout << "\nTransformed Points:\n";

    cout << "[\n";
    for (int i = 0; i < n; i++)
        cout << "  " << transformed[i].x
             << "  " << transformed[i].y
             << "  1\n";
    cout << "]\n";
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-500, 500, -500, 500);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    cout << "Enter number of points: ";
    cin >> n;

    cout << "\nEnter coordinates of points:\n";

    for (int i = 0; i < n; i++) {
        cout << "Point " << i + 1 << " (x y): ";
        cin >> original[i].x >> original[i].y;

        // Initially transformed = original
        transformed[i] = original[i];
    }

    int choice;

    cout << "\n===== 2D TRANSFORMATION MENU =====\n";
    cout << "1. Translation\n";
    cout << "2. Scaling\n";
    cout << "3. Rotation\n";
    cout << "4. X-Shear\n";
    cout << "5. Y-Shear\n";
    cout << "6. Composite Transformation\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice) {

        case 1:
            translation();
            break;

        case 2:
            scaling();
            break;

        case 3:
            rotation();
            break;

        case 4:
            xShear();
            break;

        case 5:
            yShear();
            break;

        case 6:
            composite();
            break;

        default:
            cout << "Invalid choice!";
            return 0;
    }

    printPoints();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Assignment 3 - 2D Transformations");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}