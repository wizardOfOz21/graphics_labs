/*

Алгоритм отсечения: Коэна-Сазерленда
Размерность пространства отсечения: Трехмерное
Тип отсечения: Внутреннее

*/

#include <iostream>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>

using std::cos;
using std::sin;

bool XT = 0;
bool YT = 0;
bool ZT = 0;
bool XR = 0;
bool YR = 0;
bool ZR = 0;
bool DXU = 0;
bool DXD = 0;
bool DZU = 0;
bool DZD = 0;
bool CUT = 0;

struct Point
{
    float x, y, z;
};
struct Line
{
    Point begin;
    Point end;
};
struct Cutter
{
    Point leftUp;
    Point rightDown;
};
enum Sides
{
    TOP    = 1 << 0,
    BOTTOM = 1 << 1,
    RIGHT  = 1 << 2,
    LEFT   = 1 << 3,
    FRONT  = 1 << 4,
    BACK   = 1 << 5,
};

void CreatePrism(int vNum, float *vs, float dx, float dy, float dz)
{
    glBegin(GL_POLYGON);
        glColor3ub(255, 255, 0);
        for (int i = 0; i < vNum; i++)
        {
            int s = 3 * i;
            float x = vs[s];
            float y = vs[s + 1];
            float z = vs[s + 2];
            glVertex3f(x, y, z);
        }
    glEnd();

    glBegin(GL_POLYGON);
        glColor3ub(255, 255, 255);
        for (int i = 0; i < vNum; i++)
        {
            int s = 3 * i;
            float x = vs[s];
            float y = vs[s + 1];
            float z = vs[s + 2];
            glVertex3f(x + dx, y + dy, z + dz);
        }
    glEnd();

    glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < vNum; i++)
        {
            glColor3ub(20 * i, 10 * i, 100 - 200 * i);
            int s = 3 * i;

            float x = vs[s];
            float y = vs[s + 1];
            float z = vs[s + 2];
            glVertex3f(x, y, z);
            glVertex3f(x + dx, y + dy, z + dz);
        }

        int s = 0;
        float x = vs[s];
        float y = vs[s + 1];
        float z = vs[s + 2];
        glVertex3f(x, y, z);
        glVertex3f(x + dx, y + dy, z + dz);
    glEnd();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
        CUT = !CUT;
    else if (key == GLFW_KEY_X && action == GLFW_PRESS)
        XT = !XT;
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        YT = !YT;
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        ZT = !ZT;
    else if (key == GLFW_KEY_I && action == GLFW_PRESS)
        XR = !XR;
    else if (key == GLFW_KEY_J && action == GLFW_PRESS)
        YR = !YR;
    else if (key == GLFW_KEY_K && action == GLFW_PRESS)
        ZR = !ZR;
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        DZD = !DZD;
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        DXU = !DXU;
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        DXD = !DXD;
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        DZU = !DZU;
}

GLFWwindow* init_window(int width, int height)
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
    };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Lab_5", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

void displayCutter(const Cutter &cutter)
{
    float x1 = cutter.rightDown.x;
    float y1 = cutter.rightDown.y;
    float z1 = cutter.rightDown.z;

    float x2 = cutter.leftUp.x;
    float y2 = cutter.leftUp.y;
    float z2 = cutter.leftUp.z;

    float qprism[] = {x1, y1, z1,
                      x2, y1, z1,
                      x2, y1, z2,
                      x1, y1, z2};

    CreatePrism(4, qprism, 0, y2 - y1, 0);
}

void displayLine(const Line &line)
{
    glBegin(GL_LINES);
        glColor3ub(255, 0, 0);
        glVertex3f(line.begin.x, line.begin.y, line.begin.z);
        glVertex3f(line.end.x, line.end.y, line.end.z);
    glEnd();
}

char getCode(const Cutter &cutter, Point &point)
{
    char code = 0;
    if (point.y > cutter.leftUp.y)
        code += TOP;
    else if (point.y < cutter.rightDown.y)
        code += BOTTOM;
    if (point.x > cutter.rightDown.x)
        code += RIGHT;
    else if (point.x < cutter.leftUp.x)
        code += LEFT;
    if (point.z < cutter.leftUp.z)
        code |= FRONT;
    else if (point.z > cutter.rightDown.z)
        code += BACK;
    return code;
};

Line cut(const Cutter &cutter, const Line &line)
{
    char begin_code;
    char end_code;

    bool is_inner = false;
    bool end_flag = false;

    Line result;
    result.begin = line.begin;
    result.end = line.end;

    Point &begin = result.begin;
    Point &end = result.end;

    begin_code = getCode(cutter, result.begin);
    end_code = getCode(cutter, result.end);

    do
    {
        float x, y, z;
        if (begin_code == 0 && end_code == 0)
        {
            is_inner = true;
            end_flag = true;
        }
        else if ((begin_code & end_code) != 0)
            end_flag = true;
        else
        {
            char code = begin_code ? begin_code : end_code;
            if (code & TOP)
            {
                x = begin.x + (end.x - begin.x) * (cutter.leftUp.y - begin.y) / (end.y - begin.y);
                z = begin.z + (end.z - begin.z) * (cutter.leftUp.y - begin.y) / (end.y - begin.y);
                y = cutter.leftUp.y;
            }
            else if (code & BOTTOM)
            {
                x = begin.x + (end.x - begin.x) * (cutter.rightDown.y - begin.y) / (end.y - begin.y);
                z = begin.z + (end.z - begin.z) * (cutter.rightDown.y - begin.y) / (end.y - begin.y);
                y = cutter.rightDown.y;
            }
            else if (code & RIGHT)
            {
                y = begin.y + (end.y - begin.y) * (cutter.rightDown.x - begin.x) / (end.x - begin.x);
                z = begin.z + (end.z - begin.z) * (cutter.rightDown.x - begin.x) / (end.x - begin.x);
                x = cutter.rightDown.x;
            }
            else if (code & LEFT)
            {
                y = begin.y + (end.y - begin.y) * (cutter.leftUp.x - begin.x) / (end.x - begin.x);
                z = begin.z + (end.z - begin.z) * (cutter.leftUp.x - begin.x) / (end.x - begin.x);
                x = cutter.leftUp.x;
            }
            else if (code & FRONT)
            {
                x = begin.x + (end.x - begin.x) * (cutter.leftUp.z - begin.z) / (end.z - begin.z);
                y = begin.y + (end.y - begin.y) * (cutter.leftUp.z - begin.z) / (end.z - begin.z);
                z = cutter.leftUp.z;
            }
            else if (code & BACK)
            {
                x = begin.x + (end.x - begin.x) * (cutter.rightDown.z - begin.z) / (end.z - begin.z);
                y = begin.y + (end.y - begin.y) * (cutter.rightDown.z - begin.z) / (end.z - begin.z);
                z = cutter.rightDown.z;
            }
            if (code == begin_code)
            {
                begin = std::move(Point{x, y, z});
                begin_code = getCode(cutter, begin);
            }
            else
            {
                end = std::move(Point{x, y, z});
                end_code = getCode(cutter, end);
            }
        }
    } while (!end_flag);

    if (is_inner)
        return result;
    return Line{};
};

void init_view()
{
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float perspective[] = {
        0.87, -0.09, 0.98,  0.49, 
          0 ,  0.98, 0.35,  0.17, 
         0.5,  0.15, -1.7, -0.85, 
          0 ,    0 ,   1 ,   2
    };
    glMultMatrixf(perspective);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char *argv[])
{
    int width = 1024;
    int height = 512;
    GLFWwindow* window = init_window(width, height);

    glViewport(0, 0, width, height);
    init_view();

    float xRot = 0.0f, xTrans =  0.01,
          yRot = 0.0f, yTrans =  0.01,
          zRot = 0.0f, zTrans = -0.01,
          tstep = 0.005,
          rstep = 0.01,
          dstep = 0.01;

    std::vector<Line> lines;
    Point start = Point{-0.4, 0.8, -0.5};
    lines.push_back({start, Point{0.4, -1, 0}});
    lines.push_back({start, Point{0.4, -0.4, 0.6}});
    Line &line0 = lines[0];
    Line &line1 = lines[1];
    Cutter cutter{
        Point{-0.4, 0.4, -0.5},
        Point{0.4, -0.4, 0.1}
    };

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        float rotateX[] = {
            1,     0,         0,      0, 
            0,  cos(xRot), sin(xRot), 0, 
            0, -sin(xRot), cos(xRot), 0, 
            0,     0,         0,      1,
        };
        float rotateY[] = {
            cos(yRot), 0, -sin(yRot), 0, 
                0,     1,     0,      0, 
            sin(yRot), 0,  cos(yRot), 0, 
                0,     0,     0,      1,
        };
        float translateX[] = {
              1,    0,   0,    0, 
              0,    1,   0,    0, 
              0,    0,   1,    0, 
            xTrans, 0,   0,    1,
        };
        float translateY[] = {
              1,    0,   0,    0, 
              0,    1,   0,    0, 
              0,    0,   1,    0, 
              0, yTrans, 0,    1,
        };
        float translateZ[] = {
              1,    0,   0,    0, 
              0,    1,   0,    0, 
              0,    0,   1,    0, 
              0,    0, zTrans, 1,
        };

        glLoadIdentity();
        glPushMatrix();
        glMultMatrixf(translateX);
        glMultMatrixf(translateY);
        glMultMatrixf(translateZ);
        glMultMatrixf(rotateX);
        glMultMatrixf(rotateY);

        if (XR)  xRot += rstep;
        if (YR)  yRot += rstep;
        if (XT)  xTrans += tstep;
        if (YT)  yTrans -= tstep;
        if (ZT)  zTrans += tstep;
        if (DXU) line0.end.x += dstep;
        if (DXD) line0.end.x -= dstep;
        if (DZU) line0.end.z += dstep;
        if (DZD) line0.end.z -= dstep;

        displayCutter(cutter);
        if (CUT)
        {
            for (Line line : lines)
                displayLine(cut(cutter, line));
        }
        else
        {
            for (Line line : lines)
                displayLine(line);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
