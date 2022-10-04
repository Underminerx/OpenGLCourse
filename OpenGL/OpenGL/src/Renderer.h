#pragma once

#include<GL/glew.h>

// 返回false时会触发断点
#define ASSERT(x) if(!(x)) __debugbreak();
// 定位bug位置
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);