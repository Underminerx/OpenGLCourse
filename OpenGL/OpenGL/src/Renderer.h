#pragma once

#include<GL/glew.h>

// ����falseʱ�ᴥ���ϵ�
#define ASSERT(x) if(!(x)) __debugbreak();
// ��λbugλ��
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);