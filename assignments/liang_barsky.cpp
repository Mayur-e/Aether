#include <GL/glut.h>
#include <iostream>

// Liang-Barsky Line Clipping — standalone GLUT visualiser

const float WIN_XMIN = -20.0f, WIN_XMAX = 20.0f;
const float WIN_YMIN = -15.0f, WIN_YMAX = 15.0f;

bool clipTest(float p, float q, float& t1, float& t2)
{
    if (p == 0.0f) return (q >= 0.0f);
    float r = q / p;
    if (p < 0.0f) { if (r > t2) return false; if (r > t1) t1 = r; }
    else          { if (r < t1) return false; if (r < t2) t2 = r; }
    return true;
}

bool liangBarsky(float x1, float y1, float x2, float y2,
                 float& cx1, float& cy1, float& cx2, float& cy2)
{
    float dx = x2-x1, dy = y2-y1;
    float t1 = 0.0f, t2 = 1.0f;

    if (!clipTest(-dx, x1-WIN_XMIN, t1, t2)) return false;
    if (!clipTest( dx, WIN_XMAX-x1, t1, t2)) return false;
    if (!clipTest(-dy, y1-WIN_YMIN, t1, t2)) return false;
    if (!clipTest( dy, WIN_YMAX-y1, t1, t2)) return false;

    cx1 = x1 + t1*dx;  cy1 = y1 + t1*dy;
    cx2 = x1 + t2*dx;  cy2 = y1 + t2*dy;
    return true;
}

struct Seg { float x1,y1,x2,y2; };
Seg segs[] = {
    {-40, -5,  40,  5},
    {-30, 20,  10,-20},
    { -8, -8,   8,  8},
    { 25,  5,  40, 20},
    {-35, 12,  -5,  3},
};
int numSegs = 5;

void drawAxes()
{
    glColor3f(0.4f,0.4f,0.4f);
    glBegin(GL_LINES);
      glVertex2f(-50,0); glVertex2f(50,0);
      glVertex2f(0,-40); glVertex2f(0,40);
    glEnd();
}

void drawWindow()
{
    glColor3f(1,1,1); glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
      glVertex2f(WIN_XMIN,WIN_YMIN); glVertex2f(WIN_XMAX,WIN_YMIN);
      glVertex2f(WIN_XMAX,WIN_YMAX); glVertex2f(WIN_XMIN,WIN_YMAX);
    glEnd();
    glLineWidth(1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawAxes();
    drawWindow();

    for (int i = 0; i < numSegs; ++i)
    {
        glColor3f(0.6f,0.2f,0.2f);
        glBegin(GL_LINES);
          glVertex2f(segs[i].x1,segs[i].y1);
          glVertex2f(segs[i].x2,segs[i].y2);
        glEnd();

        float cx1,cy1,cx2,cy2;
        if (liangBarsky(segs[i].x1,segs[i].y1,segs[i].x2,segs[i].y2,cx1,cy1,cx2,cy2))
        {
            glColor3f(0.2f,0.9f,1.0f);
            glLineWidth(2.5f);
            glBegin(GL_LINES);
              glVertex2f(cx1,cy1); glVertex2f(cx2,cy2);
            glEnd();
            glLineWidth(1.0f);
        }
    }
    glFlush();
}

void init()
{
    glClearColor(0.08f,0.08f,0.12f,1.0f);
    gluOrtho2D(-50,50,-40,40);
}

int main(int argc, char** argv)
{
    std::cout << "Liang-Barsky Line Clipping\n";
    std::cout << "Clip window: X["<<WIN_XMIN<<","<<WIN_XMAX<<"]  Y["<<WIN_YMIN<<","<<WIN_YMAX<<"]\n";
    std::cout << "Red = original,  Cyan = clipped portion\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 640);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Liang-Barsky Line Clipping");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
