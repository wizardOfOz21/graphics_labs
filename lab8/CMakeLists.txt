cmake_minimum_required(VERSION 3.0.0)
project(lab8 VERSION 0.1.0)

add_executable(lab8 main.cpp)
target_link_libraries(lab8 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} glfw glm::glm)
