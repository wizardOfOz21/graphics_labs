#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "util.hpp"

GLuint vbo[3];
GLuint ebo;
GLuint vao;

GLuint texture;
GLuint v_shader;
GLuint f_shader;
GLuint prog;

std::vector<glm::mat4> model_view;
std::vector<glm::mat4> projection;

enum M_MODE
{
    MODEL, 
    PROJECTION,
};

void load_identity(M_MODE mode)
{
    switch (mode)
    {
    case MODEL:
        model_view.push_back(glm::mat4(1.0f));
        break;
    case PROJECTION:
        projection.push_back(glm::mat4(1.0f));
        break;
    }
}

void push_mat(M_MODE mode)
{
    switch (mode)
    {
    case MODEL:
        model_view.push_back(model_view.back());
        break;
    case PROJECTION:
        projection.push_back(model_view.back());
        break;
    }
}

void pop_mat(M_MODE mode)
{
    switch (mode)
    {
    case MODEL:
        model_view.pop_back();
        break;
    case PROJECTION:
        projection.pop_back();
        break;
    }
}

glm::mat4& model()
{
    return model_view.back();
}

glm::mat4& proj()
{
    return projection.back();
}

glm::vec3 l_pos(0,0,0);
glm::vec3 c_pos(0,0,0);

GLint modelAttr;
GLint projAttr;
GLint lightPosAttr;
GLint camPosAttr;

GLint posAttr;
GLint tcoordAttr;
GLint textureAttr;
GLint normalAttr;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLFWwindow* init_glfw(int width, int height)
{
    if (glfwInit() != GLFW_TRUE) {
        std::cout << "Failed to initialize GLFW" << std::endl;
    };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Lab_8", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

GLuint loadTexture(const std::string& path)
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

GLuint loadShader(const std::string& path, GLuint type)
{
    std::string s = readFile(path);
    const GLchar* text = s.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    
    GLint ok;
    GLchar log[2000];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glGetShaderInfoLog(shader, 2000, nullptr, log);
        std::cout << log << std:: endl;
    }
    return shader;
};

void setUnifomLocations(GLuint prog)
{
    modelAttr    = glGetUniformLocation(prog, "model");    
    projAttr     = glGetUniformLocation(prog, "proj");
    textureAttr  = glGetUniformLocation(prog, "texture");
    lightPosAttr = glGetUniformLocation(prog, "lightPos");
    camPosAttr   = glGetUniformLocation(prog, "viewPos");

    posAttr    = glGetAttribLocation(prog, "position");
    tcoordAttr = glGetAttribLocation(prog, "texcoord");
    normalAttr = glGetAttribLocation(prog, "normal");
}

GLuint init_vao(GLuint pos, GLuint normal, GLuint tcoord)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, pos);
            glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, tcoord);
            glVertexAttribPointer(tcoordAttr, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, normal);
            glVertexAttribPointer(normalAttr, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return vao;
}

void load_prism(int n, float *data, Vector& da) {

    GLuint indices[3*n + 2*3*n + 3*n];
    GLuint* top__ind = &indices[0];
    GLuint* side_ind = &indices[3*n];
    GLuint* bott_ind = &indices[3*n + 2*3*n];
    
    GLfloat vertices[3*n + 4*3*n + 3*n];
    GLfloat* top__v = &vertices[0];
    GLfloat* side_v = &vertices[3*n];
    GLfloat* bott_v = &vertices[3*n + 4*3*n];

    GLfloat   normal[3*n + 3*4*n + 3*n];
    GLfloat* top__n  =  &normal[0];
    GLfloat* side_n  =  &normal[3*n];
    GLfloat* bott_n  =  &normal[3*n + 4*3*n];

    GLfloat textCoord[2*n];

    Vector a(&data[0]);
    Vector b(&data[3]);
    Vector c(&data[6]);
    Vector u = b - a;
    Vector v = c - a;
    Vector n_b = a*b;

    for (int i = 0; i < n; ++i) {

        GLfloat* rv = &data[3*i];
        Vector pos(rv);
        Vector::assign(&top__v[3*i], pos     );
        Vector::assign(&bott_v[3*i], pos + da);

        int next = (i+1)%n;
        GLfloat* nv = &data[3*next];
        Vector n_pos(nv);
        Vector::assign(&side_v[4*3*i + 3*0], pos       );
        Vector::assign(&side_v[4*3*i + 3*1], pos   + da);
        Vector::assign(&side_v[4*3*i + 3*2], n_pos + da);
        Vector::assign(&side_v[4*3*i + 3*3], n_pos     );

        Vector::assign(&top__n[3*i], n_b);
        Vector::assign(&bott_n[3*i], n_b);

        Vector b = n_pos - pos;
        // Vector norm = b * da;
        Vector norm = da * b;

        Vector::assign(&side_n[4*3*i + 3*0], norm);
        Vector::assign(&side_n[4*3*i + 3*1], norm);
        Vector::assign(&side_n[4*3*i + 3*2], norm);
        Vector::assign(&side_n[4*3*i + 3*3], norm);

        textCoord[2*i + 0] = (pos.x + 1)/2;
        textCoord[2*i + 1] = (pos.z + 1)/2;

        top__ind[3*i + 0] = 0;
        top__ind[3*i + 1] = i;
        top__ind[3*i + 2] = next;
        bott_ind[3*i + 0] = n + 4*n;
        bott_ind[3*i + 1] = n + 4*n + i;
        bott_ind[3*i + 2] = n + 4*n + next;

        int offset = n + 4*i;
        side_ind[2*3*i + 0] = offset + 0;
        side_ind[2*3*i + 1] = offset + 1;
        side_ind[2*3*i + 2] = offset + 3;
        side_ind[2*3*i + 3] = offset + 1;
        side_ind[2*3*i + 4] = offset + 2;
        side_ind[2*3*i + 5] = offset + 3;
    }   

    glGenBuffers(3, vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textCoord), textCoord, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glUseProgram(prog);

        setUnifomLocations(prog);
        vao = init_vao(vbo[0], vbo[1], vbo[2]);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureAttr, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
}

void draw(int n)
{
    glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_VERTEX_ARRAY);

            glUseProgram(prog);
                glUniformMatrix4fv(modelAttr, 1, GL_FALSE, glm::value_ptr(model()));
                glUniform3fv(lightPosAttr, 1, glm::value_ptr(l_pos));
                glBindVertexArray(vao);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glEnableVertexAttribArray(posAttr);
                    glEnableVertexAttribArray(tcoordAttr);
                    glEnableVertexAttribArray(normalAttr);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                        glDrawElements(GL_TRIANGLES, 3*n + 2*3*n + 3*n, GL_UNSIGNED_INT, nullptr);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    glDisableVertexAttribArray(posAttr);
                    glDisableVertexAttribArray(tcoordAttr);
                    glDisableVertexAttribArray(normalAttr);
                    glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
            glUseProgram(0);
            
        glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void load_resources() {

    texture = loadTexture("../images/ice.jpg");
    v_shader = loadShader("../lab8/shaders/v_shader.glsl", GL_VERTEX_SHADER);
    f_shader = loadShader("../lab8/shaders/f_shader.glsl", GL_FRAGMENT_SHADER);

    prog = glCreateProgram();
    glAttachShader(prog, v_shader);
    glAttachShader(prog, f_shader);
    glLinkProgram(prog);

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
}

void init_view()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    load_identity(PROJECTION);
    proj() = glm::frustum(-1,1,-1,1,1,8);

    load_identity(MODEL);
    model() = glm::translate(model(), glm::vec3{0,0,-4});
    model() = glm::rotate(model(), (float)M_PI/6, glm::vec3{1,0,0});

    l_pos = model()*glm::vec4(l_pos, 1);
}

int main(int argc, char *argv[]) {
    
    GLuint width = 800;
    GLuint height = 600;
    GLFWwindow* window = init_glfw(width, height);

    glewInit();
    load_resources();
    glViewport(0, 0, width, height);
    init_view();

    float a = 2;
    float q2 = sqrt(2);
    float b = q2*a;

    float sprism[] = {
        -a/2, 0,  a/2, 
         a/2, 0,  a/2, 
         a/2, 0, -a/2,   
        -a  , 0,   -a,
        -2*a, 0, -a/2,
        };
    float cool_prism[] = {
         -(a/2+b), 0,  -a/2, 
         - a/2, 0,  -(a/2 + b), 
           a/2, 0,  -(a/2 + b),   
          a/2+b,0, -a/2,
          a/2+b,0, a/2,
           a/2, 0,  a/2 + b,
         - a/2, 0,  a/2 + b, 
         -(a/2+b), 0, a/2, 
    };
    float qprism[] = {
        -a/2, 0,  a/2, 
         a/2, 0,  a/2, 
         a/2, 0, -a/2,   
        -a/2, 0, -a/2,
        };
    int vnum = 4;

    Vector da(-1, -2, -1);
    Point p(Vector{0, 2, 0}, Vector{0, 0, 0}, Vector{0, -0.0005, 0}, Vector{1, 0, 1});
    load_prism(vnum, qprism, da);

    float tetha = 0;

    glUseProgram(prog);
        glUniformMatrix4fv(projAttr, 1, GL_FALSE, glm::value_ptr(proj()));
        glUniform3fv(camPosAttr, 1, glm::value_ptr(c_pos));
    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2, 0.2, 0.4, 0);    //outColor = texture2D(texture, Texcoord);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        push_mat(MODEL);
            model() = glm::rotate(model(), tetha, glm::vec3{0,1,0});
            push_mat(MODEL);
                model() = glm::translate(model(), glm::vec3{p.pos.x,p.pos.y,p.pos.z});
                l_pos = model()*glm::vec4(l_pos, 1);
            pop_mat(MODEL);
            draw(vnum);
            l_pos = glm::vec3(-1,0,-1);
        pop_mat(MODEL);

        tetha += M_PI/360;
        p.recount();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
