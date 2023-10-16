#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>

bool XT = 0, YT = 0, ZT = 0,
     XR = 0, YR = 0, ZR = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    GLFWwindow *window = glfwCreateWindow(width, height, "Lab_3", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

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
            glColor3ub(20 * i, 10 * i, 100 - 2 * i);
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

void init_view()
{
    glEnable(GL_DEPTH_TEST);

    float perspective[] = {
        0.87, -0.09, 0.98,  0.49, 
          0,   0.98, 0.35,  0.17, 
        0.5,   0.15, -1.7, -0.85, 
         0,      0,    1,     2 ,
    };
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMultMatrixf(perspective);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char *argv[])
{
    int width  = 800;
    int height = 600;
    GLFWwindow* window = init_window(width, height);

    glViewport(0, 0, width, height);
    init_view();

    float xRot   =  0.0f, xTrans =  0.01,
          yRot   =  0.0f, yTrans =  0.01,
          zRot   =  0.0f, zTrans = -0.01,
          rstep  =  0.01, tstep  =  0.005;

    float a = 0.7;
    float tprism[] = {
        0.0, 0.0, 0.0, 
         a,   0,  0.0, 
        a/2, 0.0, -a ,
    };
    float qprism[] = {
        0, 0,  0,
        a, 0,  0,
        a, 0, -a,
        0, 0, -a,
    };
    float fprism[] = {
        0, 0,    0,
        a, 0,    0,
        a, 0,   -a,
        0, 0, -2*a,
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
        float a = 0.5;
        float scale[] = {
              a,   0,   0,   0, 
              0,   a,   0,   0, 
              0,   0,   a,   0, 
              0,   0,   0,   1,
        };
        float translateDown[] = {
              1,    0,   0,    0, 
              0,    1,   0,    0, 
              0,    0,   1,    0, 
            -2.5,   0,   0,    1,
        };

        glLoadIdentity();
        glPushMatrix();
            glMultMatrixf(translateX);
            glMultMatrixf(translateY);
            glMultMatrixf(translateZ);
            glMultMatrixf(rotateX);
            glMultMatrixf(rotateY);
            CreatePrism(4, qprism, 0, 0.5, 0.5);
        glPopMatrix();
        glPushMatrix();
            glMultMatrixf(scale);
            glMultMatrixf(translateDown);
            CreatePrism(4, qprism, 0, 0.5, 0.5);
        glPopMatrix();

        if (XR) xRot += rstep;
        if (YR) yRot += rstep;
        if (XT) xTrans += tstep;
        if (YT) yTrans -= tstep;
        if (ZT) zTrans += tstep;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
