#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

// Bezier Curve — standalone GLUT visualiser
// Shows a cubic Bezier with interactive control points (click to move).
// Also renders a Hermite comparison curve.

struct Vec2 { float x, y; };

// ---- De Casteljau ----
Vec2 bezierEval(const std::vector<Vec2>& pts, float t)
{
    std::vector<Vec2> w = pts;
    int n = (int)w.size();
    for (int r = 1; r < n; ++r)
        for (int i = 0; i < n-r; ++i)
        {
            w[(size_t)i].x = w[(size_t)i].x*(1-t) + w[(size_t)(i+1)].x*t;
            w[(size_t)i].y = w[(size_t)i].y*(1-t) + w[(size_t)(i+1)].y*t;
        }
    return w[0];
}

// ---- Hermite basis ----
Vec2 hermiteEval(Vec2 p0, Vec2 m0, Vec2 p1, Vec2 m1, float t)
{
    float t2=t*t, t3=t2*t;
    float h00= 2*t3-3*t2+1, h10=t3-2*t2+t, h01=-2*t3+3*t2, h11=t3-t2;
    return { p0.x*h00+m0.x*h10+p1.x*h01+m1.x*h11,
             p0.y*h00+m0.y*h10+p1.y*h01+m1.y*h11 };
}

// ---- Control points (Bezier) ----
std::vector<Vec2> ctrl = { {-30,-20}, {-10,35}, {15,35}, {35,-20} };
int dragIdx = -1;

// ---- Draw helpers ----
void drawBezier()
{
    glColor3f(0.2f, 0.8f, 1.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 200; ++i)
    {
        Vec2 p = bezierEval(ctrl, (float)i/200.0f);
        glVertex2f(p.x, p.y);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawHermite()
{
    // Use first and last control point as endpoints, derive tangents from ctrl hull
    Vec2 p0 = ctrl[0], p1 = ctrl[3];
    Vec2 m0 = { 3*(ctrl[1].x-ctrl[0].x), 3*(ctrl[1].y-ctrl[0].y) };
    Vec2 m1 = { 3*(ctrl[3].x-ctrl[2].x), 3*(ctrl[3].y-ctrl[2].y) };

    glColor3f(1.0f, 0.5f, 0.1f);
    glLineWidth(2.0f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 200; ++i)
    {
        Vec2 p = hermiteEval(p0, m0, p1, m1, (float)i/200.0f);
        glVertex2f(p.x, p.y);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.0f);
}

void drawControlHull()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : ctrl) glVertex2f(p.x, p.y);
    glEnd();

    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < (int)ctrl.size(); ++i)
    {
        glColor3f(i == 0 || i == 3 ? 1.0f : 0.9f,
                  i == 0 || i == 3 ? 0.9f : 0.6f, 0.2f);
        glVertex2f(ctrl[(size_t)i].x, ctrl[(size_t)i].y);
    }
    glEnd();
    glPointSize(1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    // Grid
    glColor3f(0.15f,0.15f,0.2f);
    glBegin(GL_LINES);
    for (int i=-40;i<=40;i+=10)
    {
        glVertex2f((float)i,-42); glVertex2f((float)i,42);
        glVertex2f(-42,(float)i); glVertex2f(42,(float)i);
    }
    glEnd();
    // Axes
    glColor3f(0.4f,0.4f,0.4f);
    glBegin(GL_LINES);
      glVertex2f(-45,0); glVertex2f(45,0);
      glVertex2f(0,-45); glVertex2f(0,45);
    glEnd();

    drawHermite();
    drawBezier();
    drawControlHull();
    glFlush();
}

void mouse(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON) return;
    // Convert window coords to world
    int vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
    float wx = -45.0f + (float)x/vp[2]*90.0f;
    float wy =  45.0f - (float)y/vp[3]*90.0f;

    if (state == GLUT_DOWN)
    {
        dragIdx = -1;
        float best = 4.0f;
        for (int i = 0; i < (int)ctrl.size(); ++i)
        {
            float d = std::sqrt((ctrl[(size_t)i].x-wx)*(ctrl[(size_t)i].x-wx)
                              + (ctrl[(size_t)i].y-wy)*(ctrl[(size_t)i].y-wy));
            if (d < best) { best=d; dragIdx=i; }
        }
    }
    else dragIdx = -1;
}

void motion(int x, int y)
{
    if (dragIdx < 0) return;
    int vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
    ctrl[(size_t)dragIdx].x = -45.0f + (float)x/vp[2]*90.0f;
    ctrl[(size_t)dragIdx].y =  45.0f - (float)y/vp[3]*90.0f;
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.07f, 0.07f, 0.11f, 1.0f);
    gluOrtho2D(-45, 45, -45, 45);
}

int main(int argc, char** argv)
{
    std::cout << "Bezier Curve (de Casteljau) + Hermite Curve\n";
    std::cout << "Blue  = Bezier cubic  |  Orange dashed = equivalent Hermite\n";
    std::cout << "Drag yellow control points with the mouse.\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(150, 60);
    glutCreateWindow("Bezier & Hermite Curves");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}
