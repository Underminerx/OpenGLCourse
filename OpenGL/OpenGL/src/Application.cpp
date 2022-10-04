#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

// ����һ���Է��������ַ���
struct ShaderProgramSource 
{
    std::string VertexSource;
    std::string FragmentSource;
};

// ȥ�ļ��л�ȡshaderԴ����
static ShaderProgramSource ParseShader(const std::string& filepath)
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

static GLuint CompileShader(GLuint type, const std::string& source)
{
    // ��������������ɫ����������ɫ��
    GLuint id = glCreateShader(type);
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
static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
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


GLint main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // OpenGL 3.3 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // ����OpenGL�����ļ�Ϊ��ģʽ     -> GLFW_OPENGL_CORE_PROFILE     ���봴��һ������������� ��vaoȷ��Ŀ����Ⱦ���� (�����vao δ��������)
    // ����OpenGL�����ļ�Ϊ����ģʽ    -> GLFW_OPENGL_COMPAT_PROFILE   ��һ��Ĭ�ϵĶ���������� ���Բ�ʹ��vao ֱ�Ӱ����������ʹ��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ����ˢ�¼��
    glfwSwapInterval(3);

    // δ��ʼ���ɹ�����
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }

    // ����̨���OpenGL�汾��Ϣ
    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        // ���嶥������
        GLfloat positions[] = {
            -0.5f, -0.5f,   // 0
             0.5f, -0.5f,   // 1
             0.5f,  0.5f,   // 2
            -0.5f,  0.5f,   // 3
        };

        // ���ݶ�����������
        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // ʹ��GLFW_OPENGL_CORE_PROFILEʱ��һ����������Ϊָ������
        GLuint vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));


        VertexBuffer vb(positions, 4 * 2 * sizeof(GLfloat));

        //// ���建��������
        //GLuint buffer;
        //// ���建����
        //GLCall(glGenBuffers(1, &buffer));
        //// �󶨻�����id
        //GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        //// �����ݴ��뻺����(������)
        //GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), positions, GL_STATIC_DRAW));

        // ʹ�������������ݶ�GPU�ɼ�
        GLCall(glEnableVertexAttribArray(0));
        // buffer������ ��ά���㣬 �����������ͣ� �Ƿ��һ���� ���˶���ڶ������ԵĲ���, ��һ�����Եĳ���
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0));        // ����vertexBuffer��vao

        IndexBuffer ib(indices, 6);

        //// ���建�������� 
        //GLuint ibo;     // Index Buffer Object
        //// ���建����
        //GLCall(glGenBuffers(1, &ibo));
        //// �󶨻�����id
        //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        //// �����ݴ��뻺����(������)
        //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW));

        //// ������ɫ����ʼ������
        //std::string vertexShader =
        //    "#version 330 core\n"
        //    "\n"
        //    "layout(location = 0) in vec4 position;\n"
        //    "\n"
        //    "void main()\n"
        //    "{\n"
        //    "   gl_Position = position;\n"
        //    "}\n";

        //// ������ɫ����ʼ������
        //std::string fragmentShader =
        //    "#version 330 core\n"
        //    "\n"
        //    "layout(location = 0) out vec4 color;\n"
        //    "\n"
        //    "void main()\n"
        //    "{\n"
        //    "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        //    "}\n";

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        //std::cout << "VERTEX" << std::endl;
        //std::cout << source.VertexSource << std::endl;
        //std::cout << "FRAGMENT" << std::endl;
        //std::cout << source.FragmentSource << std::endl;


        GLuint shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));


        // Ѱ����ɫ���е�ͳһ����
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));     // ��������λ��
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));


        // �������buffer
        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));

        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));


            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

            GLCall(glBindVertexArray(vao));

            ib.Bind();
            // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

            // �κ��������嶼��Ҫ���޷�����
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        // ���shader
        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}