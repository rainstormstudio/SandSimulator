/**
 * @file R2DEngine.hpp
 * @author Daniel Hongyu Ding
 * @brief Rainstorm 2D Engine
 * @version 0.2
 * @date 2021-01-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#ifndef R2DENGINE_HPP
#define R2DENGINE_HPP

// standard libraries
#include <cstdint>
#include <cmath>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <algorithm>

#if USE_OPENGL
// opengl related
#define GLFW_INCLUDE_NONE
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif USE_SDL2
    #ifdef __linux__
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_mixer.h"
    #include "SDL2/SDL_ttf.h"
    #elif _WIN32
    #include "SDL.h"
    #include "SDL_image.h"
    #include "SDL_mixer.h"
    #include "SDL_ttf.h"
    #endif
#else
// default using opengl
#define USE_OPENGL 1
#define GLFW_INCLUDE_NONE
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

/*
STATIC_ASSERT(expr) assert expr at compile-time

ASSERT(expr) assert expr at run-time

DEBUG_MSG(expr) put expr to standard error

DEBUG_MSG_INDENT(expr, depth) put expr with indentation based on depth
    to standard error

DEBUG_LINE() put a horizontal line to standard error

DEBUG_LINE_INDENT(depth) put a horizontal line with indentation
    based on depth to standard error
*/

#if DEBUG_ENABLED

    #ifdef __cplusplus
        #if __cplusplus >= 201103L
            #define STATIC_ASSERT(expr) \
                static_assert(expr, "static assert failed:"#expr)
        #endif
    #endif

    #if defined(__i386__) || defined(__x86_64__)
        #define debugbreak() __asm__ __volatile__("int3")
    #elif defined(__aarch64__)
        #define debugbreak() __asm__ __volatile__(".inst 0xd4200000")
    #elif defined(__arm__)
        #define debugbreak() __asm__ __volatile__(".inst 0xe7f001f0")
    #endif

    #define ASSERT(expr) \
    if (expr) {}  \
    else {  \
        Debug::msg(#expr, __FILE__, __LINE__);  \
        debugbreak(); \
    }

    #define DEBUG_MSG(expr) Debug::msg(expr)
    #define DEBUG_MSG_INDENT(expr, depth) Debug::msg(expr, depth)
    #define DEBUG_LINE() Debug::line()
    #define DEBUG_LINE_INDENT(depth) Debug::line(depth)

#else
    #define STATIC_ASSERT(expr)
    #define ASSERT(expr) 
    #define DEBUG_MSG(expr) 
    #define DEBUG_MSG_INDENT(expr, depth) 
    #define DEBUG_LINE()
    #define DEBUG_LINE_INDENT(depth)
#endif

#define DEBUG_ERROR(expr) Debug::error(expr)
#define DEBUG_ERROR_INDENT(expr, depth) Debug::error(expr, depth);

namespace Debug {
    void msg(const char* expr, const char* file, int line) {
        std::cerr << "[" << file << "] at line " << line << ": " << expr << std::endl; 
    }

    void msg(const char* expr, int depth = 0) {
        for (int i = 0; i < depth; i ++) {
            std::cerr << "    ";
        }
        std::cerr << expr << std::endl;
    }

    void error(const char* expr, int depth = 0) {
        for (int i = 0; i < depth; i ++) {
            std::cerr << "    ";
        }
        std::cerr << "ERROR: " << expr << std::endl;
    }

    void line(int depth = 0) {
        for (int i = 0; i < depth; i ++) {
            std::cerr << "    ";
        }
        for (int i = 0; i < 80 - depth * 4; i ++) {
            std::cerr << "-";
        }
        std::cerr << std::endl;
    }
};

#if USE_OPENGL
// glfw callbacks
void glfwErrorCallback(int error, const char* description);
void glfwFramebufferSizeCallback(GLFWwindow* window, int screenWidth, int screenHeight);

// default shaders
static const char *vShader = "              \n\
#version 410                                \n\
                                            \n\
layout (location = 0) in vec2 position;     \n\
layout (location = 1) in vec2 tex;          \n\
                                            \n\
out vec2 texCoord;                          \n\
                                            \n\
void main() {                               \n\
    gl_Position = vec4(position, 0.0, 1.0); \n\
    texCoord = tex;                         \n\
}";

static const char *fShader = "                      \n\
#version 410                                        \n\
                                                    \n\
in vec2 texCoord;                                   \n\
out vec4 gl_FragColor;                              \n\
uniform sampler2D theTexture;                       \n\
                                                    \n\
void main() {                                       \n\
    gl_FragColor = texture(theTexture, texCoord);   \n\
}";
#endif

class R2DEngine {
private:
    // game
    bool loop;

    // graphics
#if USE_OPENGL
    GLFWwindow* window;
    GLuint shader;
    GLuint bufferTexture;
    GLubyte* bufferData;
    GLuint vao, ibo, vbo;
#elif USE_SDL2
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* bufferTexture;
    uint8_t* bufferData;
#endif

    // events
#if USE_SDL2
    SDL_Event event;
#endif

protected:
    // game
    struct Coord {
        uint32_t x = 0;
        uint32_t y = 0;
        Coord(uint32_t x = 0, uint32_t y = 0) : x(x), y(y) {}
    };

    struct Color {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;
        Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    };
    
    // graphics
    int32_t screenWidth;
    int32_t screenHeight;
    int32_t innerWidth;
    int32_t innerHeight;
    std::string windowTitle;

    // events
    enum InputState {
        UNKNOWN,
        PRESS,
        RELEASE,
        REPEAT
    };
    double mousePosX;
    double mousePosY;
    
private:
    void gameLoop();

    void clearBuffer();
    void swapBuffers();

#if USE_OPENGL
    void addShader(GLuint program, const char* shaderCode, GLenum shaderType);
    void compileShaders();
#endif

public:
    R2DEngine();
    virtual ~R2DEngine();

public:
    // user interfaces
    virtual bool onCreate() = 0;
    virtual bool onUpdate(double deltaTime) = 0;
    virtual bool onDestroy() {
        return true;
    }

public:
    // game
    bool construct(int32_t screenWidth = 800, int32_t screenHeight = 600, int32_t innerWidth = 800, int32_t innerHeight = 600);
    void init();

public:
    // events
    InputState getKeyState(int key) const;
    InputState getMouseState(int mouseButton) const;


public:
    // graphics
    void drawPoint(Coord coord, Color color);
    void drawLine(Coord coord1, Coord coord2, Color color);
};

#if USE_OPENGL
void glfwErrorCallback(int error, const char* description) {
    DEBUG_MSG(description);
}

void glfwFramebufferSizeCallback(GLFWwindow* window, int screenWidth, int screenHeight) {
    glViewport(0, 0, screenWidth, screenHeight);
}
#endif


R2DEngine::R2DEngine() {
#if USE_OPENGL
    window = nullptr;
    shader = 0;
    vao = 0;
    vbo = 0;
    ibo = 0;
    bufferData = nullptr;
    bufferTexture = 0;
#elif USE_SDL2
    window = nullptr;
    renderer = nullptr;
    bufferData = nullptr;
#endif
    loop = false;

    screenWidth = 0;
    screenHeight = 0;
    innerWidth = 0;
    innerHeight = 0;
    windowTitle = "R2DEngine";
}

R2DEngine::~R2DEngine() {}

bool R2DEngine::construct(int32_t screenWidth, int32_t screenHeight, int32_t innerWidth, int innerHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->innerWidth = innerWidth;
    this->innerHeight = innerHeight;

#if USE_OPENGL
    if (!glfwInit()) {
        DEBUG_ERROR("Failed to initialize GLFW");
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwSetErrorCallback(glfwErrorCallback);

    window = glfwCreateWindow(this->screenWidth, this->screenHeight, windowTitle.c_str(), nullptr, nullptr);
    if (!window) {
        DEBUG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        DEBUG_ERROR("Failed to initialize GLEW");
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, this->screenWidth, this->screenHeight);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, this->innerWidth, this->innerHeight, 0.0, 1.0, -1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    DEBUG_MSG("window constructed");

    compileShaders();
    DEBUG_MSG("shaders compiled");

    bufferData = new GLubyte[innerWidth * innerHeight * 4];
    memset(bufferData, 0, sizeof(GLubyte) * innerWidth * innerHeight * 4);
    glGenTextures(1, &bufferTexture);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, innerWidth, innerHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bufferData);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLfloat vertices[] = {
        -1.0f, 1.0f,    0.0f, 0.0f,
        1.0f, 1.0f,     1.0f, 0.0f,
        1.0f, -1.0f,    1.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 1.0f
    };
    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 4, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 4, (GLvoid*)(sizeof(vertices[0]) * 2));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    DEBUG_MSG("buffer generated");
#elif USE_SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        DEBUG_ERROR("SDL initialization failed: ");
        DEBUG_ERROR(SDL_GetError());
        return false;
    } else {
        window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        SDL_SetWindowFullscreen(window, 0);
        SDL_RaiseWindow(window);
        if (!window) {
            DEBUG_ERROR("Failed to create window: ");
            DEBUG_ERROR(SDL_GetError());
            return false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
                DEBUG_ERROR("Failed to load SDL_image: ");
                DEBUG_ERROR(IMG_GetError());
                return false;
            }
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                DEBUG_ERROR("Failed to load SDL_mixer: ");
                DEBUG_ERROR(Mix_GetError());
                return false;
            }
        }
    }
    bufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        innerWidth,
        innerHeight
    );
    bufferData = new uint8_t[innerWidth * innerHeight * 4];
    memset(bufferData, 0, sizeof(uint8_t) * innerWidth * innerHeight * 4);
#endif

    return true;
}

void R2DEngine::init() {
    DEBUG_MSG("init");
    loop = true;
    gameLoop();
}

void R2DEngine::clearBuffer() {
#if USE_OPENGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    memset(bufferData, 0, sizeof(GLubyte) * innerWidth * innerHeight * 4);
#elif USE_SDL2
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    memset(bufferData, 0, sizeof(uint8_t) * innerWidth * innerHeight * 4);
#endif
}

void R2DEngine::swapBuffers() {
#if USE_OPENGL
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, innerWidth, innerHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bufferData);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    glfwSwapBuffers(window);
#elif USE_SDL2
    SDL_UpdateTexture(bufferTexture, nullptr, (void*)bufferData, innerWidth * 4);
    SDL_RenderCopy(renderer,  bufferTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
#endif
}

#if USE_OPENGL
void R2DEngine::addShader(GLuint program, const char* shaderCode, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    const GLchar *theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(shader, 1, theCode, codeLength);
    glCompileShader(shader);

    GLint test;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
    if (!test) {
        DEBUG_ERROR("shader compilation failed:");
        GLchar log[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        DEBUG_ERROR(log);
        return;
    }
    glAttachShader(program, shader);
}

void R2DEngine::compileShaders() {
    shader = glCreateProgram();
    if (!shader) {
        DEBUG_ERROR("failed to create shader program");
        return;
    }

    addShader(shader, vShader, GL_VERTEX_SHADER);
    addShader(shader, fShader, GL_FRAGMENT_SHADER);

    glLinkProgram(shader);

    GLint test;
    glGetProgramiv(shader, GL_LINK_STATUS, &test);
    if (!test) {
        DEBUG_ERROR("failed to link program:");
        GLchar log[1024] = {0};
        glGetProgramInfoLog(shader, sizeof(log), nullptr, log);
        DEBUG_ERROR(log);
        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(shader, GL_VALIDATE_STATUS, &test);
    if (!test) {
        DEBUG_ERROR("failed to validate program:");
        GLchar log[1024] = {0};
        glGetProgramInfoLog(shader, sizeof(log), nullptr, log);
        DEBUG_ERROR(log);
        return;
    }
}
#endif

void R2DEngine::gameLoop() {
    if (!onCreate()) {
        loop = false;
    }

    double deltaTime = 0.0;
#if USE_OPENGL
    double time_a = glfwGetTime();
    double time_b = glfwGetTime();
#elif USE_SDL2
    uint64_t time_a = SDL_GetPerformanceCounter();
    uint64_t time_b = SDL_GetPerformanceCounter();
#endif

    DEBUG_MSG("game loop start");
    while (loop) {
        while (loop) {
#if USE_OPENGL
            time_b = glfwGetTime();
            deltaTime = time_b - time_a;
            while (deltaTime < 0.001) {
                time_b = glfwGetTime();
                deltaTime = time_b - time_a;
            }
            time_a = time_b;
            std::string title = windowTitle + " - FPS: " + std::to_string(1.0 / deltaTime);
            glfwSetWindowTitle(window, title.c_str());

            glfwPollEvents();
            if (glfwWindowShouldClose(window)) {
                loop = false;
                break;
            }
            glfwGetCursorPos(window, &mousePosX, &mousePosY);
            mousePosX = round(mousePosX / screenWidth * innerWidth);
            mousePosY = round(mousePosY / screenHeight * innerHeight);
            GLint m_viewport[4];
            glGetIntegerv(GL_VIEWPORT, m_viewport);
            screenWidth = m_viewport[2];
            screenHeight = m_viewport[3];
#elif USE_SDL2
            time_b = SDL_GetPerformanceCounter();
            deltaTime = (double)((time_b - time_a) / (double)SDL_GetPerformanceFrequency());
            time_a = time_b;
            std::string title = windowTitle + " - FPS " + std::to_string(1.0 / deltaTime);
            SDL_SetWindowTitle(window, title.c_str());
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT: {
                        loop = false;
                        break;
                    }
                    case SDL_MOUSEMOTION: {
                        mousePosX = event.motion.x;
                        mousePosY = event.motion.y;
                        mousePosX = round(mousePosX / screenWidth * innerWidth);
                        mousePosY = round(mousePosY / screenHeight * innerHeight);
                        break;
                    }
                }
            }
            SDL_GetWindowSize(window, &screenWidth, &screenHeight);
#endif
            
            clearBuffer();
            if (!onUpdate(deltaTime)) {
                loop = false;
            }
            swapBuffers();
        }

        if (!onDestroy()) {
            loop = true;
        }
    }

    DEBUG_MSG("game loop end");

#if USE_OPENGL
    if (ibo != 0) {
        glDeleteBuffers(1, &ibo);
        ibo = 0;
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        glDeleteBuffers(1, &vao);
        vao = 0;
    }
    glDeleteTextures(1, &bufferTexture);
    delete bufferData;
            
    glfwDestroyWindow(window);
    glfwTerminate();

    DEBUG_MSG("glfw destroyed");
#elif USE_SDL2
    SDL_DestroyTexture(bufferTexture);
    delete bufferData;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    DEBUG_MSG("SDL destroyed");
#endif
}

void R2DEngine::drawPoint(Coord coord, Color color) {
    if (0 <= coord.x && coord.x < innerWidth && 0 <= coord.y && coord.y < innerHeight) {
        bufferData[coord.y * innerWidth * 4 + coord.x * 4 + 0] = color.r;
        bufferData[coord.y * innerWidth * 4 + coord.x * 4 + 1] = color.g;
        bufferData[coord.y * innerWidth * 4 + coord.x * 4 + 2] = color.b;
        bufferData[coord.y * innerWidth * 4 + coord.x * 4 + 3] = color.a;
    }
}

R2DEngine::InputState R2DEngine::getKeyState(int key) const {
#if USE_OPENGL
    int state = glfwGetKey(window, key);
    if (state == GLFW_PRESS) {
        return PRESS;
    } else if (state == GLFW_RELEASE) {
        return RELEASE;
    } else if (state == GLFW_REPEAT) {
        return REPEAT;
    }
#endif
    return UNKNOWN;
}

R2DEngine::InputState R2DEngine::getMouseState(int mouseButton) const {
#if USE_OPENGL
    int state = glfwGetMouseButton(window, mouseButton);
    if (state == GLFW_PRESS) {
        return PRESS;
    } else if (state == GLFW_RELEASE) {
        return RELEASE;
    } else if (state == GLFW_REPEAT) {
        return REPEAT;
    }
#endif
    return UNKNOWN;
}

#endif