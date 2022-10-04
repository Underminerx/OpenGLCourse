#include "Renderer.h"

#include <iostream>

// 清除之前的所有错误信息
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// 返回false触发断点并输出错误信息
bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}