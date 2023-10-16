cmake_minimum_required(VERSION 3.0.0)
project(lab4 VERSION 0.1.0)

add_executable(lab4 main.cpp)
include_directories(.)
target_link_libraries(lab4 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
