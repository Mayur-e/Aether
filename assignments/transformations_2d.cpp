#include <GL/glut.h>
#include <iostream>
#include <cmath>

// 2D Transformations — standalone GLUT visualiser
// Draws a triangle then applies translation, rotation, scaling, shear,
// and reflection. Press SPACE to cycle through transforms.

struct Vec2 { float x, y; };
struct Mat3 { float m[3][3]; };

Mat3 identity()
{
    return {{
        {1,0,0},
        {0,1,0},
        {0,0,1}
    }};
}

Vec2 applyMat(const Mat3& M, Vec2 p)
{
    float x = M.m[0][0]*p.x + M.m[0][1]*p.y + M.m[0][2];
    float y = M.m[1][0]*p.x + M.m[1][1]*p.y + M.m[1][2];
    return {x, y};
}

// Build matrices
Mat3 makeTrans(float tx, float ty)
{
    return {{ {1,0,tx}, {0,1,ty}, {0,0,1} }};
}
Mat3 makeRot(float deg)
{
    float r=deg*3.14159f/180.0f, c=cosf(r), s=sinf(r);
    return {{ {c,-s,0}, {s,c,0}, {0,0,1} }};
}
Mat3 makeScale(float sx, float sy)
{
    return {{ {sx,0,0}, {0,sy,0}, {0,0,1} }};
}
Mat3 makeShear(float shx, float shy)
{
    return {{ {1,shx,0}, {shy,1,0}, {0,0,1} }};
}
Mat3 makeReflX()
{
    return {{ {1,0,0}, {0,-1,0}, {0,0,1} }};
}

// Triangle (original)
Vec2 triOrig[3] = { {-10,-8}, {10,-8}, {0,12} };

int curTransform = 0;  // 0=identity, 1=translate, 2=rotate, 3=scale, 4=shear, 5=reflX
const char* labels[] = {
    "Identity (original)",
    "Translation  (+15, +10)",
    "Rotation     (45 deg CCW)",
    "Scaling      (2x, 1.5y)",
    "Shear        (shx=0.5)",
    "Reflection   (about X-axis)"
};
const int NUM_TRANSFORMS = 6;

Mat3 getTransform()
{
    switch (curTransform)
    {
        case 1: return makeTrans(15, 10);
        case 2: return makeRot(45);
        case 3: return makeScale(2.0f, 1.5f);
        case 4: return makeShear(0.5f, 0.0f);
        case 5: return makeReflX();
        default: return identity();
    }
}

void drawTriangle(Vec2* pts, float r, float g, float b, bool outline=false)
{
    glColor3f(r, g, b);
    glBegin(outline ? GL_LINE_LOOP : GL_TRIANGLES);
    for (int i = 0; i < 3; ++i)
        glVertex2f(pts[i].x, pts[i].y);
    glEnd();
}

void printPoints(const char* label, Vec2* pts)
{
    std::cout << "\n" << label << "\n";
    for (int i = 0; i < 3; ++i)
        std::cout << "  P" << i << ": (" << pts[i].x << ", " << pts[i].y << ")\n";
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Axes
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
      glVertex2i(-45,0); glVertex2i(45,0);
      glVertex2i(0,-45); glVertex2i(0,45);
    glEnd();

    // Original (dim)
    drawTriangle(triOrig, 0.3f, 0.3f, 0.7f, true);

    // Transformed
    Mat3 M = getTransform();
    Vec2 triNew[3];
    for (int i = 0; i < 3; ++i)
        triNew[i] = applyMat(M, triOrig[i]);
    drawTriangle(triNew, 0.2f, 0.9f, 0.4f, false);
    drawTriangle(triNew, 0.1f, 0.5f, 0.2f, true);

    glFlush();
}

void keyboard(unsigned char key, int, int)
{
    if (key == ' ')
    {
        curTransform = (curTransform + 1) % NUM_TRANSFORMS;
        std::cout << "\nTransform: " << labels[curTransform] << "\n";
        Mat3 M = getTransform();
        Vec2 triNew[3];
        for (int i = 0; i < 3; ++i) triNew[i] = applyMat(M, triOrig[i]);
        printPoints("Transformed vertices:", triNew);
        glutPostRedisplay();
    }
}

void init()
{
    glClearColor(0.07f, 0.07f, 0.11f, 1.0f);
    gluOrtho2D(-45, 45, -45, 45);
}

int main(int argc, char** argv)
{
    std::cout << "2D Transformations Demo\n";
    std::cout << "Press SPACE to cycle through: identity, translate, rotate, scale, shear, reflect\n";
    std::cout << "Blue outline = original triangle | Green = transformed\n\n";
    printPoints("Original vertices:", triOrig);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(150, 60);
    glutCreateWindow("2D Transformations");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
