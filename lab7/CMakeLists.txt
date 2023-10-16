cmake_minimum_required(VERSION 3.0.0)
project(lab7 VERSION 0.1.0)

add_executable(lab7_basic basic.cpp)
add_executable(lab7_optimized optimized.cpp)

target_link_libraries(lab7_basic ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)
target_link_libraries(lab7_optimized ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} glfw)
