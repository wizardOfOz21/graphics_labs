#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "util.hpp"

GLuint texture;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLuint load_texture(const std::string& path)
{
    GLuint texture;
    int width, height, cnt;
    unsigned char *data =
        stbi_load(path.c_str(), &width, &height, &cnt, 0);

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
    return texture;
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

void load_resources() {
    texture = load_texture("../images/11.png");
}

void load_prism(int vNum, float *vs, const Vector& da, GLuint *vbo, GLuint* ebo) {
    float verts[vNum*6];

    GLuint t_side[(vNum + 1) * 2];
    GLuint top[vNum];
    GLuint bottom[vNum];

    float textCoord[vNum * 2];

    for (int i = 0; i < vNum; ++i) {

        int s = 3 * i;
        float x = vs[s];
        float y = vs[s + 1];
        float z = vs[s + 2];
        verts[s]   = x;  verts[3*vNum + s]   = x + da.x;
        verts[s+1] = y;  verts[3*vNum + s+1] = y + da.y;
        verts[s+2] = z;  verts[3*vNum + s+2] = z + da.z;

        t_side[2 * i]     = i;
        t_side[2 * i + 1] = i + vNum;

        top[i]    = i;
        bottom[i] = vNum + i;

        textCoord[2*i] = x + 0.5;
        textCoord[2*i + 1] = z - 0.5;
    }

    t_side[2*vNum] = 0;
    t_side[2*vNum + 1] = vNum;

    glGenBuffers(2, vbo);
    glGenBuffers(3, ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textCoord), textCoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(top), top,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(t_side), t_side,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bottom), bottom,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw(int vNum, const GLuint *vbo, const GLuint* ebo)
{
    glEnable(GL_TEXTURE_2D);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexPointer(3, GL_FLOAT, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
            glColor3f(0,0,1);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
                glDrawElements(GL_TRIANGLE_STRIP, (vNum+1)*2, GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glNormal3f(0,1,0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
                glDrawElements(GL_TRIANGLE_FAN, vNum, GL_UNSIGNED_INT, nullptr);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glColor3f(1,1,1);
            glBindTexture(GL_TEXTURE_2D, texture);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
                    glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
                        glDrawElements(GL_TRIANGLE_FAN, vNum, GL_UNSIGNED_INT, nullptr);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindTexture(GL_TEXTURE_2D, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void init_view()
{
    glewInit();
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-1, 1, -1, 1, 2, 6);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(0, 0, -4);
    glRotatef(30, 1, 0, 0);
    glScalef(1.5, 1.5, 1.5);

    glPointSize(5);

    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
    glEnable(GL_LIGHT0);
    GLfloat diffuse[] = {2, 2, 2};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.2);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.7);
}

int main() {
    int width = 800;
    int height = 600;
    GLFWwindow* window = init_window(width, height);

    glViewport(0, 0, width, height);
    load_resources();
    init_view();

    float a = 1;
    float qprism[] = {
        -a/2, 0,  a/2, 
         a/2, 0,  a/2, 
         a/2, 0, -a/2,  
        -a/2, 0, -a/2,
    };

    Vector da(0, -0.5, 0);
    Point p(
        Vector{0, 1.5, 0}, 
        Vector{0, 0, 0}, 
        Vector{0, -0.0005, 0},
        Vector{1, 0.1, 1}
    );

    float light_pos[] = {0, 0.1, 0, 1};

    GLuint vbo[2];
    GLuint ebo[3];
    load_prism(4, qprism, da, vbo, ebo);

    GLuint point = glGenLists(1);

    glNewList(point, GL_COMPILE);
        glBegin(GL_POINTS);
            glColor3f(1, 1, 1);
            glVertex3f(0, 0, 0);
        glEnd();
    glEndList();

    float tetha = 0;

    TimeCounter tc(QL);
    
    while (!glfwWindowShouldClose(window)) {
        tc.start();
        glClearColor(0.2, 0.2, 0.4, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();
            glRotatef(tetha, 0, 1, 0);
            draw(4, vbo, ebo);
            glPushMatrix();
                glTranslatef(p.pos.x, p.pos.y, p.pos.z);
                glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                glCallList(point);
            glPopMatrix();
        glPopMatrix();
        tetha += 0.5;
        p.recount();

        glfwSwapBuffers(window);
        glfwPollEvents();

        tc.end();
        if (tc.count == QL - 1) break;
    }
    glfwTerminate();
    std::cout << tc.average() << " ms" << std::endl;
    return 0;
}
