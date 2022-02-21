cmake_minimum_required(VERSION 3.16)
project(toucanmodelling)

if(${CMAKE_VERSION} VERSION_LESS 3.16)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# find OpenMP
find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME} main.cpp mesh.cpp )

if(WIN32 AND MSVC)
    target_link_libraries(${PROJECT_NAME} -lOpenGL32 -lfreeGLUT -lGLU32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(GLUT REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    set(OpenGL_GL_PREFERENCE GLVND)
    target_link_libraries(${PROJECT_NAME} OpenGL::OpenGL OpenGL::GLU GLUT::GLUT GLEW::GLEW /home/thegreentoucan/fromsource/gcc-11/lib64/libstdc++.a)
endif()

