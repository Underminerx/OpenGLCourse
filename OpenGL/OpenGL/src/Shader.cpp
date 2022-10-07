#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
    
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}


// 去文件中获取shader源代码
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];    // 存放两个着色器
    ShaderType type = ShaderType::NONE;     // 默认当前着色器类型为空

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(GLint)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    // 决定创建顶点着色器或像素着色器
    unsigned int id = glCreateShader(type);
    // 读入着色器内容
    const GLchar* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    // 编译着色器
    glCompileShader(id);

    // 错误处理
    GLint result;
    // 获取编译状态
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    // 如果编译出错则进行处理
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));   // 栈内存 不需要手动释放
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader !";
        std::cout << std::endl;
        // 打印出错相关信息 行数...
        std::cout << message << std::endl;
        // 清除shader 可选
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// 创建着色器
GLuint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // 状态索引
    GLuint program = glCreateProgram();
    // 编译着色器源码
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // 链接shader
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // 清除编译后的shader（编译后的shader已经链接）可选
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
    GLCall(glUseProgram(0));
}

// 修改全局变量
void Shader::SetUniform1f(const std::string& name, float value)
{
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

// 定位到全局变量位置
unsigned int Shader::GetUniformLocation(const std::string& name)
{
    // 若在hash表中找到了 则直接返回其值
    if (m_UniformLoacationCache.find(name) != m_UniformLoacationCache.end())    // 找到end代表没找到 没找到end代表找到了
        return m_UniformLoacationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLoacationCache[name] = location;
    return location;
}
