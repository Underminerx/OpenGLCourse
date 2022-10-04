#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

// 用于一次性返回两个字符串
struct ShaderProgramSource 
{
    std::string VertexSource;
    std::string FragmentSource;
};

// 去文件中获取shader源代码
static ShaderProgramSource ParseShader(const std::string& filepath)
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

static GLuint CompileShader(GLuint type, const std::string& source)
{
    // 决定创建顶点着色器或像素着色器
    GLuint id = glCreateShader(type);
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
static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
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


GLint main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // OpenGL 3.3 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 设置OpenGL配置文件为核模式     -> GLFW_OPENGL_CORE_PROFILE     必须创建一个顶点数组对象 绑定vao确定目标渲染物体 (建议绑定vao 未关心性能)
    // 设置OpenGL配置文件为兼容模式    -> GLFW_OPENGL_COMPAT_PROFILE   有一个默认的顶点数组对象 可以不使用vao 直接绑定数组对象来使用
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

    // 设置刷新间隔
    glfwSwapInterval(3);

    // 未初始化成功报错
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }

    // 控制台输出OpenGL版本信息
    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        // 定义顶点数组
        GLfloat positions[] = {
            -0.5f, -0.5f,   // 0
             0.5f, -0.5f,   // 1
             0.5f,  0.5f,   // 2
            -0.5f,  0.5f,   // 3
        };

        // 根据顶点索引绘制
        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // 使用GLFW_OPENGL_CORE_PROFILE时绑定一个缓冲区作为指定布局
        GLuint vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));


        VertexBuffer vb(positions, 4 * 2 * sizeof(GLfloat));

        //// 定义缓冲区长度
        //GLuint buffer;
        //// 定义缓冲区
        //GLCall(glGenBuffers(1, &buffer));
        //// 绑定缓冲区id
        //GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        //// 将数据存入缓冲区(带个数)
        //GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), positions, GL_STATIC_DRAW));

        // 使缓冲区顶点数据对GPU可见
        GLCall(glEnableVertexAttribArray(0));
        // buffer索引， 几维顶点， 顶点数据类型， 是否归一化， 到此顶点第二个属性的步幅, 另一个属性的长度
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0));        // 绑定了vertexBuffer与vao

        IndexBuffer ib(indices, 6);

        //// 定义缓冲区长度 
        //GLuint ibo;     // Index Buffer Object
        //// 定义缓冲区
        //GLCall(glGenBuffers(1, &ibo));
        //// 绑定缓冲区id
        //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        //// 将数据存入缓冲区(带个数)
        //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW));

        //// 顶点着色器初始化内容
        //std::string vertexShader =
        //    "#version 330 core\n"
        //    "\n"
        //    "layout(location = 0) in vec4 position;\n"
        //    "\n"
        //    "void main()\n"
        //    "{\n"
        //    "   gl_Position = position;\n"
        //    "}\n";

        //// 像素着色器初始化内容
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


        // 寻找着色器中的统一变量
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));     // 检索变量位置
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));


        // 解绑所有buffer
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

            // 任何索引缓冲都需要是无符号数
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

        // 清除shader
        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}