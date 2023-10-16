cmake_minimum_required(VERSION 3.0.0)
project(lab5 VERSION 0.1.0)

add_executable(lab5 main.cpp)
target_link_libraries(lab5 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
