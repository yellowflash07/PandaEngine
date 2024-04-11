#pragma once

#include <glfw/glfw3.h>
#include <iostream>
// Define a macro to check OpenGL errors
#define CHECK_GL_ERROR() \
    do { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            std::cerr << "OpenGL Error (" << error << "): at" << __FILE__ << ":" << __LINE__ << std::endl; \
            switch(error) { \
                case GL_INVALID_ENUM: \
                    std::cerr << "GL_INVALID_ENUM"; \
                    break; \
                case GL_INVALID_VALUE: \
                    std::cerr << "GL_INVALID_VALUE"; \
                    break; \
                case GL_INVALID_OPERATION: \
                    std::cerr << "GL_INVALID_OPERATION"; \
                    break; \
                case GL_OUT_OF_MEMORY: \
                    std::cerr << "GL_OUT_OF_MEMORY"; \
                    break; \
                case GL_INVALID_FRAMEBUFFER_OPERATION: \
                    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION"; \
                    break; \
                default: \
                    std::cerr << "Unknown error"; \
                    break; \
            } \
            std::cerr << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    } while(0)