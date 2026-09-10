#include <GL/glut.h>
#include <iostream>
#include <cmath>

// Cohen-Sutherland — standalone GLUT visualiser
// Demonstrates clipping of multiple line segments against a visible window.

// ---- Window (clip region) ----
const float WIN_XMIN = -20.0f, WIN_XMAX = 20.0f;
const float WIN_YMIN = -15.0f, WIN_YMAX = 15.0f;

// ---- Region codes ----
const int INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;

int computeCode(float x, float y)
{
    int code = INSIDE;
    if      (x < WIN_XMIN) code |= LEFT;
    else if (x > WIN_XMAX) code |= RIGHT;
    if      (y < WIN_YMIN) code |= BOTTOM;
    else if (y > WIN_YMAX) code |= TOP;
    return code;
}

// Returns true if the segment is (at least partially) visible after clipping.
bool cohenSutherlandClip(float& x1, float& y1, float& x2, float& y2)
{
    int c1 = computeCode(x1, y1);
    int c2 = computeCode(x2, y2);

    while (true)
    {
        if (!(c1 | c2))     return true;       // both inside
        if  (c1 & c2)       return false;      // trivially outside

        int cOut = c1 ? c1 : c2;
        float x, y;

        if      (cOut & TOP)    { x = x1 + (x2-x1)*(WIN_YMAX-y1)/(y2-y1); y = WIN_YMAX; }
        else if (cOut & BOTTOM) { x = x1 + (x2-x1)*(WIN_YMIN-y1)/(y2-y1); y = WIN_YMIN; }
        else if (cOut & RIGHT)  { y = y1 + (y2-y1)*(WIN_XMAX-x1)/(x2-x1); x = WIN_XMAX; }
        else                    { y = y1 + (y2-y1)*(WIN_XMIN-x1)/(x2-x1); x = WIN_XMIN; }

        if (cOut == c1) { x1=x; y1=y; c1=computeCode(x1,y1); }
        else            { x2=x; y2=y; c2=computeCode(x2,y2); }
    }
}

// ---- Test segments (original) ----
struct Seg { float x1,y1,x2,y2; };
Seg segs[] = {
    {-40,-5,  40, 5},   // cuts through both sides
    {-30,20,  10,-20},  // cuts top & bottom
    {-10,-10, 10, 10},  // entirely inside
    { 25, 5,  40, 20},  // entirely outside (right)
    {-35,12, -5,  3},   // enters from left
};
int numSegs = 5;

void drawAxes()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
      glVertex2f(-50,0); glVertex2f(50,0);
      glVertex2f(0,-50); glVertex2f(0,50);
    glEnd();
}

void drawClipWindow()
{
    glColor3f(1,1,1);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
      glVertex2f(WIN_XMIN, WIN_YMIN);
      glVertex2f(WIN_XMAX, WIN_YMIN);
      glVertex2f(WIN_XMAX, WIN_YMAX);
      glVertex2f(WIN_XMIN, WIN_YMAX);
    glEnd();
    glLineWidth(1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawClipWindow();

    for (int i = 0; i < numSegs; ++i)
    {
        // Draw original line (dim red)
        glColor3f(0.6f, 0.2f, 0.2f);
        glBegin(GL_LINES);
          glVertex2f(segs[i].x1, segs[i].y1);
          glVertex2f(segs[i].x2, segs[i].y2);
        glEnd();

        // Clip and draw visible portion (bright green)
        float x1=segs[i].x1, y1=segs[i].y1, x2=segs[i].x2, y2=segs[i].y2;
        if (cohenSutherlandClip(x1,y1,x2,y2))
        {
            glColor3f(0.0f, 1.0f, 0.3f);
            glLineWidth(2.5f);
            glBegin(GL_LINES);
              glVertex2f(x1,y1); glVertex2f(x2,y2);
            glEnd();
            glLineWidth(1.0f);
        }
    }

    glFlush();
}

void init()
{
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    gluOrtho2D(-50, 50, -40, 40);
}

int main(int argc, char** argv)
{
    std::cout << "Cohen-Sutherland Line Clipping\n";
    std::cout << "Clip window: X["<<WIN_XMIN<<","<<WIN_XMAX<<"]  Y["<<WIN_YMIN<<","<<WIN_YMAX<<"]\n";
    std::cout << "Red = original,  Green = clipped portion\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 640);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Cohen-Sutherland Line Clipping");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
