cmake_minimum_required(VERSION 3.0.0)
project(lab2 VERSION 0.1.0)

add_executable(lab2 main.cpp)
target_link_libraries(lab2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
