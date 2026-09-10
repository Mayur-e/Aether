#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstring>

// 3D Transformations & Projections — standalone GLUT visualiser
// Shows a wireframe cube with 3D transforms + selectable projection mode.
// Press 1-5 to switch projection; SPACE to cycle transforms; WASD to rotate view.

// ---- Simple 4x4 matrix (column-major) ----
struct Mat4 { float m[4][4]; };
struct Vec4 { float x,y,z,w; };

Mat4 identity4()
{
    Mat4 M; memset(M.m, 0, sizeof(M.m));
    M.m[0][0]=M.m[1][1]=M.m[2][2]=M.m[3][3]=1; return M;
}
Vec4 mul(const Mat4& M, Vec4 v)
{
    return {
        M.m[0][0]*v.x+M.m[0][1]*v.y+M.m[0][2]*v.z+M.m[0][3]*v.w,
        M.m[1][0]*v.x+M.m[1][1]*v.y+M.m[1][2]*v.z+M.m[1][3]*v.w,
        M.m[2][0]*v.x+M.m[2][1]*v.y+M.m[2][2]*v.z+M.m[2][3]*v.w,
        M.m[3][0]*v.x+M.m[3][1]*v.y+M.m[3][2]*v.z+M.m[3][3]*v.w,
    };
}
Mat4 mul(const Mat4& A, const Mat4& B)
{
    Mat4 C; memset(C.m,0,sizeof(C.m));
    for(int r=0;r<4;r++) for(int c=0;c<4;c++)
        for(int k=0;k<4;k++) C.m[r][c]+=A.m[r][k]*B.m[k][c];
    return C;
}

Mat4 makeRotX(float deg)
{
    float r=deg*3.14159f/180, c=cosf(r), s=sinf(r);
    Mat4 M=identity4(); M.m[1][1]=c;M.m[1][2]=-s;M.m[2][1]=s;M.m[2][2]=c; return M;
}
Mat4 makeRotY(float deg)
{
    float r=deg*3.14159f/180, c=cosf(r), s=sinf(r);
    Mat4 M=identity4(); M.m[0][0]=c;M.m[0][2]=s;M.m[2][0]=-s;M.m[2][2]=c; return M;
}

// Projection modes
int  projMode   = 1;   // 1=perspective, 2=ortho, 3=oblique, 4=isometric, 5=axonometric
float rotX=25, rotY=30;
int curTransform = 0;
const char* transLabels[] = { "Identity", "RotateX(30)", "RotateY(45)", "Scale(1.5x)" };

// Unit cube vertices (-0.5 to 0.5)
float cube[8][3] = {
    {-.5,-.5,-.5}, {.5,-.5,-.5}, {.5,.5,-.5}, {-.5,.5,-.5},
    {-.5,-.5, .5}, {.5,-.5, .5}, {.5,.5, .5}, {-.5,.5, .5}
};
int edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},  // back
    {4,5},{5,6},{6,7},{7,4},  // front
    {0,4},{1,5},{2,6},{3,7}   // sides
};

// Project a 3D point to 2D screen space given projection mode
void project(float x, float y, float z, float& sx, float& sy)
{
    const float d = 3.0f; // eye distance for perspective
    switch (projMode)
    {
        case 1: // Perspective
            sx = x*d/(d+z); sy = y*d/(d+z); break;
        case 2: // Orthographic (parallel)
            sx = x; sy = y; break;
        case 3: // Oblique (cavalier, 45°)
            sx = x + 0.5f*z*cosf(3.14159f/4); sy = y + 0.5f*z*sinf(3.14159f/4); break;
        case 4: // Isometric
        {
            float ix = x-z, iy = (x+z)*0.5f - y;
            sx = ix*0.7f; sy = -iy*0.7f; break;
        }
        case 5: // Axonometric (dimetric)
        {
            float ax=cosf(0.5236f), ay=sinf(0.5236f);
            sx = x - z*ax; sy = y - z*ay; break;
        }
        default: sx=x; sy=y;
    }
}

const char* projNames[] = {"","Perspective","Orthographic","Oblique (cavalier)","Isometric","Dimetric"};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Build transform for current demo
    Mat4 T = identity4();
    if (curTransform == 1) T = makeRotX(30);
    if (curTransform == 2) T = makeRotY(45);
    if (curTransform == 3) { T=identity4(); T.m[0][0]=T.m[1][1]=T.m[2][2]=1.5f; }

    // View rotation (user-controlled)
    Mat4 view = mul(makeRotX(rotX), makeRotY(rotY));
    Mat4 M    = mul(view, T);

    // Draw axes
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
      glVertex2f(-40,0); glVertex2f(40,0);
      glVertex2f(0,-40); glVertex2f(0,40);
    glEnd();

    // Draw cube edges
    glColor3f(0.3f, 0.85f, 1.0f);
    glLineWidth(1.5f);
    for (int e=0;e<12;e++)
    {
        Vec4 v0={cube[edges[e][0]][0],cube[edges[e][0]][1],cube[edges[e][0]][2],1};
        Vec4 v1={cube[edges[e][1]][0],cube[edges[e][1]][1],cube[edges[e][1]][2],1};
        v0=mul(M,v0); v1=mul(M,v1);

        float sx0,sy0,sx1,sy1;
        project(v0.x,v0.y,v0.z,sx0,sy0);
        project(v1.x,v1.y,v1.z,sx1,sy1);
        // Scale up for visibility
        glBegin(GL_LINES);
          glVertex2f(sx0*25,sy0*25);
          glVertex2f(sx1*25,sy1*25);
        glEnd();
    }
    glLineWidth(1.0f);
    glFlush();
}

void keyboard(unsigned char key, int, int)
{
    if (key==' ') { curTransform=(curTransform+1)%4; std::cout<<"Transform: "<<transLabels[curTransform]<<"\n"; }
    if (key=='w') rotX-=5;
    if (key=='s') rotX+=5;
    if (key=='a') rotY-=5;
    if (key=='d') rotY+=5;
    if (key>='1'&&key<='5')
    {
        projMode=key-'0';
        std::cout<<"Projection: "<<projNames[projMode]<<"\n";
    }
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.07f,0.07f,0.11f,1.0f);
    gluOrtho2D(-45,45,-45,45);
}

int main(int argc, char** argv)
{
    std::cout << "3D Transformations & Projections Demo\n";
    std::cout << "Press 1-5 to switch projections: 1=Perspective 2=Ortho 3=Oblique 4=Isometric 5=Dimetric\n";
    std::cout << "Press SPACE to cycle 3D transforms | WASD to rotate view\n\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(150, 60);
    glutCreateWindow("3D Transformations & Projections");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
