cmake_minimum_required(VERSION 3.0.0)
project(lab6 VERSION 0.1.0)

add_executable(lab6 main.cpp)
target_link_libraries(lab6 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
