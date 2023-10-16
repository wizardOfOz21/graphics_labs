#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "util.hpp"

unsigned int texture;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    GLFWwindow *window = glfwCreateWindow(width, height, "Lab_6", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

void load_texture()
{
    int width, height, cnt;
    unsigned char *data = stbi_load("../images/11.png", &width, &height, &cnt, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, (cnt == 4) ? GL_RGBA : GL_RGB, width, height,
                 0, (cnt == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void init_view()
{
    // включаем буффер глубины
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // Задает границы области видимости в ортогональной проекции
        // glOrtho(-2,2, -2,2, -1,1);
        // задает перспективную проекцию
        // -1, 1, -1, 1 - границы ближней плоскости отсечения по x иy
        // 2, 6 - расстояния до ближней и дальней плоскостей отсечения по z
        glFrustum(-1, 1, -1, 1, 2, 6);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(0, 0, -4);
    glRotatef(30, 1, 0, 0);
    glScalef(1.5,1.5,1.5);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    glEnable(GL_LIGHT0);
    GLfloat diffuse[] = {2, 2, 2};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.2);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.7);
}

void сreate_prism(int vNum, float *vs, float dx, float dy, float dz)
{
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);

        for (int i = 0; i < vNum; i++)
        {
            int s = 3 * i;
            float x = vs[s];
            float y = vs[s + 1];
            float z = vs[s + 2];
            glVertex3f(x + dx, y + dy, z + dz);
        }

    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_QUAD_STRIP);

        for (int i = 0; i < vNum; i++)
        {
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

void draw(float a)
{
    static float colors[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    };

    static float normal[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    };

    float vertices[] = {
        -a/2, 0,  a/2, 
         a/2, 0,  a/2, 
         a/2, 0, -a/2,  
        -a/2, 0, -a/2,
    };
    
    static float texCoord[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    };

    static GLuint index[] = {
        0, 1, 2, 
        2, 3, 0
    };

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
        glNormalPointer(GL_FLOAT, 0, normal);

        glNormal3f(0, 0, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

            glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, &index);

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
};

int main()
{
    int width = 800;
    int height = 600;
    GLFWwindow* window = init_window(width, height);

    load_texture();

    glViewport(0, 0, width, height);

    init_view();

    float a = 1;
    float tprism[] = {
          0, 0,  0, 
          a, 0,  0, 
        a/2, 0, -a,
    };
    float fprism[] = {
        0,   0,      0,
        a,   0,      0,
        a,   0,     -a,
        0,   0, -2 * a
    };
    float qprism[] = {
        -a/2, 0,  a/2, 
         a/2, 0,  a/2, 
         a/2, 0, -a/2,  
        -a/2, 0, -a/2,
    };

    float tetha = 0;
    
    Point p(
        Vector{  0,   1.5,    0}, 
        Vector{  0,     0,    0}, 
        Vector{  0, -0.0005,    0}, 
        Vector{  1,     0,    1}
    );

    TimeCounter tc(QL);

    while (!glfwWindowShouldClose(window))
    {
        tc.start();
        glClearColor(0.2, 0.2, 0.4, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();

            glRotatef(tetha, 0, 1, 0);
            сreate_prism(4, qprism, 0, -0.5, 0);

            glColor3f(1, 1, 1);
            draw(a);

            glPushMatrix();

                glTranslatef(p.pos.x, p.pos.y, p.pos.z);
                float position[] = {0, 0, 0, 1};
                glLightfv(GL_LIGHT0, GL_POSITION, position);

                glPointSize(5);
                glBegin(GL_POINTS);
                    glColor3f(1, 1, 1);
                    glVertex3f(0, 0, 0);
                glEnd();

            glPopMatrix();

        glPopMatrix();
        tetha+= 0.5;

        p.recount();

        glfwSwapBuffers(window);
        glfwPollEvents();

        tc.end();
        if (tc.count == QL-1) break;
    }
    glfwTerminate();
    std::cout << tc.average() << " ms"<< std::endl;
    return 0;
}
