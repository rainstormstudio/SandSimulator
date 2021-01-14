cmake_minimum_required(VERSION 3.9)

project(R2DEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    include_directories(
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS} 
        ${SDL2_IMAGE_INCLUDE_DIR} 
        ${SDL2_TTF_INCLUDE_DIR}
        ${SDL2_MIXER_INCLUDE_DIR}
    )
endif()

file(GLOB SRC_CPP_FILES "./src/*.cpp")
file(GLOB HEADER_FILES "./src/*.hpp")

add_executable(
    ${PROJECT_NAME}
    ${SRC_CPP_FILES}
    ${HEADER_FILES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC "${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}"
)

if(UNIX)
    target_link_libraries(
        ${PROJECT_NAME}
        OpenGL::GL
        ${GLEW_LIBRARIES}
        glfw
        ${SDL2_LIBRARY} 
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

