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


// ȥ�ļ��л�ȡshaderԴ����
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];    // ���������ɫ��
    ShaderType type = ShaderType::NONE;     // Ĭ�ϵ�ǰ��ɫ������Ϊ��

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
    // ��������������ɫ����������ɫ��
    unsigned int id = glCreateShader(type);
    // ������ɫ������
    const GLchar* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    // ������ɫ��
    glCompileShader(id);

    // ������
    GLint result;
    // ��ȡ����״̬
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    // ��������������д���
    if (result == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));   // ջ�ڴ� ����Ҫ�ֶ��ͷ�
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader !";
        std::cout << std::endl;
        // ��ӡ���������Ϣ ����...
        std::cout << message << std::endl;
        // ���shader ��ѡ
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// ������ɫ��
GLuint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // ״̬����
    GLuint program = glCreateProgram();
    // ������ɫ��Դ��
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // ����shader
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // ���������shader��������shader�Ѿ����ӣ���ѡ
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

// �޸�ȫ�ֱ���
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

// ��λ��ȫ�ֱ���λ��
unsigned int Shader::GetUniformLocation(const std::string& name)
{
    // ����hash�����ҵ��� ��ֱ�ӷ�����ֵ
    if (m_UniformLoacationCache.find(name) != m_UniformLoacationCache.end())    // �ҵ�end����û�ҵ� û�ҵ�end�����ҵ���
        return m_UniformLoacationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLoacationCache[name] = location;
    return location;
}
