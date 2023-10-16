cmake_minimum_required(VERSION 3.0.0)
project(lab3 VERSION 0.1.0)

add_executable(lab3 main.cpp)
target_link_libraries(lab3 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
