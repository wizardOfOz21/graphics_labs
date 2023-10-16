/*
A1. построчного сканирования многоугольника с упорядоченным списком ребер;

Б1. целочисленный алгоритм Брезенхема с устранением ступенчатости;

В. Реализовать необходимые вспомогательные алгоритмы (растеризации отрезка) с
модификациями, обеспечивающими корректную работу основного алгоритма.
(Целочисленный алгоритм Брезенхема)

Г. Ввод исходных данных каждого из алгоритмов производится интерактивно с помощью
клавиатуры и/или мыши. Предусмотреть также возможность очистки области вывода (отмены
ввода).

Д. Растеризацию производить в специально выделенном для этого буфере в памяти с
последующим копированием результата в буфер кадра OpenGL. Предусмотреть возможность
изменение размеров окна.

*/

#include <iostream>
#include <GLFW/glfw3.h>
#include <math.h>
#include <utility>
#include <field.hpp>

const double PI = 3.141592653589793;

enum State
{
    POLYGON = 0,
    FILL = 1,
};
struct WindowContext
{
    Field &f;
    Polygon &p;
    State s;
    std::vector<std::pair<int, int>> points;
};
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        Polygon &p = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->p;
        for (int i = 0; i < p.size(); ++i)
        {
            Point &point = p[i];
            std::cout << "Point{ " << point.x << " , " << point.y << " }" << std::endl;
        }
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        Polygon &p = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->p;
        p.data.push_back(p[0]);
        p.isFinal = true;
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        Polygon &p = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->p;
        Field &f = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->f;
        std::vector<std::pair<int, int>> &points = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->points;
        std::cout << "clear\n";
        p.clear();
        points.clear();
        f.clear();
        static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->s = POLYGON;
        p.isFinal = false;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        int dw = 20;
        glfwSetWindowSize(window, width + dw, height);
        Field &f = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->f;
        f.resize(width + dw, height);
        glViewport(0, 0, width + dw, height);
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->s = FILL;
    }
}
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    switch (static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->s)
    {
    case POLYGON:
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xpos /= PIXEL_SIZE;
            ypos /= PIXEL_SIZE;
            Polygon &p = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->p;
            if (p.data.size() != 0)
            {
                p.data.push_back(Point{static_cast<int>(xpos), static_cast<int>(height / PIXEL_SIZE - ypos)});
                p.data.push_back(Point{static_cast<int>(xpos), static_cast<int>(height / PIXEL_SIZE - ypos)});
            }
            else
            {
                p.data.push_back(Point{static_cast<int>(xpos), static_cast<int>(height / PIXEL_SIZE - ypos)});
            }
            std::cout << "Point{ " << (int)xpos << " , " << (int)(height / PIXEL_SIZE - ypos) << " }" << std::endl;
        }
    break;
    case FILL:
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xpos /= PIXEL_SIZE;
            ypos /= PIXEL_SIZE;
            std::vector<std::pair<int, int>> &points = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->points;
            points.push_back(std::pair<int, int>{xpos, height / PIXEL_SIZE - ypos});
        }
        break;
    }
};
void window_size_callback(GLFWwindow *window, int width, int height)
{
    Field &f = static_cast<WindowContext *>(glfwGetWindowUserPointer(window))->f;
    f.resize(width, height);
    glViewport(0, 0, width, height);
};
GLFWwindow* init_window(int width, int height)
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
    };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Lab_4", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    return window;
}

void draw_line_test(int x0, int y0, float R, float &phi, Field &f)
{
    f.clear();
    int x = x0 + R * cos(phi);
    int y = y0 + R * sin(phi);
    phi += 0.01;
    glDrawPixels(f.get_width(), f.get_height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, f.get_buffer());
}

int main(int argc, char *argv[])
{
    int width  = 800;
    int height = 600;
    GLFWwindow* window = init_window(width, height);
    glViewport(0, 0, width, height);

    State state = POLYGON;
    Field f(width, height);
    Polygon p;
    WindowContext context{f, p, POLYGON};
    float phi = 0;
    glfwSetWindowUserPointer(window, &context);

    int lastNum = 0;
    int pointsNum = 0;
    while (!glfwWindowShouldClose(window))
    {
        switch (context.s)
        {
        case POLYGON:
            if (p.size() > lastNum)
            {
                if (p.size() % 2 == 0)
                    f.put_and_fill(p, p.size());
                else
                    f.put_and_fill(p, p.size() - 1);
            }
            break;
        case FILL:
            if (context.points.size() > pointsNum)
            {
                for (std::pair<int, int> point : context.points)
                {
                    f.put_pixel(point.first, point.second, COLOR);
                }
            }
            break;
        }

        glDrawPixels(f.get_width(), f.get_height(), GL_LUMINANCE, GL_UNSIGNED_BYTE, f.get_buffer());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwPollEvents();
}
