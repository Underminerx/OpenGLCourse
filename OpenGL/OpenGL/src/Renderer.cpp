#include "Renderer.h"

#include <iostream>

// ���֮ǰ�����д�����Ϣ
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

// ����false�����ϵ㲢���������Ϣ
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