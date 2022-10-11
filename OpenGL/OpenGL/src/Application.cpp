#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestClearColor.h"

//// 用于一次性返回两个字符串
//struct ShaderProgramSource 
//{
//    std::string VertexSource;
//    std::string FragmentSource;
//};
//
//// 去文件中获取shader源代码
//static ShaderProgramSource ParseShader(const std::string& filepath)
//{
//    std::ifstream stream(filepath);
//
//    enum class ShaderType
//    {
//        NONE = -1, VERTEX = 0, FRAGMENT = 1
//    };
//
//    std::string line;
//    std::stringstream ss[2];    // 存放两个着色器
//    ShaderType type = ShaderType::NONE;     // 默认当前着色器类型为空
//
//    while (getline(stream, line))
//    {
//        if (line.find("#shader") != std::string::npos)
//        {
//            if (line.find("vertex") != std::string::npos)
//            {
//                type = ShaderType::VERTEX;
//            }
//            else if (line.find("fragment") != std::string::npos)
//            {
//                type = ShaderType::FRAGMENT;
//            }
//        }
//        else
//        {
//            ss[(GLint)type] << line << '\n';
//        }
//    }
//
//    return { ss[0].str(), ss[1].str() };
//}
//
//static GLuint CompileShader(GLuint type, const std::string& source)
//{
//    // 决定创建顶点着色器或像素着色器
//    GLuint id = glCreateShader(type);
//    // 读入着色器内容
//    const GLchar* src = source.c_str();
//    glShaderSource(id, 1, &src, nullptr);
//    // 编译着色器
//    glCompileShader(id);
//
//    // 错误处理
//    GLint result;
//    // 获取编译状态
//    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
//    // 如果编译出错则进行处理
//    if (result == GL_FALSE) 
//    {
//        GLint length;
//        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//        GLchar* message = (GLchar*)alloca(length * sizeof(GLchar));   // 栈内存 不需要手动释放
//        glGetShaderInfoLog(id, length, &length, message);
//        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader !";
//        std::cout << std::endl;
//        // 打印出错相关信息 行数...
//        std::cout << message << std::endl;
//        // 清除shader 可选
//        glDeleteShader(id);
//        return 0;
//    }
//
//    return id;
//}
//
//// 创建着色器
//static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
//{
//    // 状态索引
//    GLuint program = glCreateProgram();
//    // 编译着色器源码
//    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
//    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
//
//    // 链接shader
//    glAttachShader(program, vs);
//    glAttachShader(program, fs);
//    glLinkProgram(program);
//    glValidateProgram(program);
//
//    // 清除编译后的shader（编译后的shader已经链接）可选
//    glDeleteShader(vs);
//    glDeleteShader(fs);
//
//    return program;
//}

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
    window = glfwCreateWindow(1280, 720, "test", NULL, NULL);
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

        /*
        // 定义顶点数组
        GLfloat positions[] = {
           -50.0f,  -50.0f,  0.0f,  0.0f,    // 0 左下
            50.0f,  -50.0f,  2.0f,  0.0f,    // 1 右下
            50.0f,   50.0f,  2.0f,  2.0f,    // 2 右上
           -50.0f,   50.0f,  0.0f,  2.0f     // 3 左上
        };

        //// 定义顶点数组(大一点的图)
        //GLfloat positions[] = {
        //     340.0f,   60.0f,  0.0f,  0.0f,    // 0 左下
        //     940.0f,   60.0f,  2.0f,  0.0f,    // 1 右下
        //     940.0f,  660.0f,  2.0f,  2.0f,    // 2 右上
        //     340.0f,  660.0f,  0.0f,  2.0f     // 3 左上
        //};


        // 根据顶点索引绘制
        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        */
        GLCall(glEnable(GL_BLEND));
        // 翻转alpha值 (1 - α)
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //// 使用GLFW_OPENGL_CORE_PROFILE时绑定一个缓冲区作为指定布局
        //GLuint vao;
        //GLCall(glGenVertexArrays(1, &vao));
        //GLCall(glBindVertexArray(vao));

        /*
        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(GLfloat));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        //// 定义缓冲区长度
        //GLuint buffer;
        //// 定义缓冲区
        //GLCall(glGenBuffers(1, &buffer));
        //// 绑定缓冲区id
        //GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        //// 将数据存入缓冲区(带个数)
        //GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), positions, GL_STATIC_DRAW));

        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
        
        glm::mat4 view  =  glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));        // 视窗向左移动100单位
        
        //// 定义缓冲区长度 
        //GLuint ibo;     // Index Buffer Object
        //// 定义缓冲区
        //GLCall(glGenBuffers(1, &ibo));
        //// 绑定缓冲区id
        //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        //// 将数据存入缓冲区(带个数)
        //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW));
        //
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
        //
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
        
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        // ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        // GLuint shader = CreateShader(source.VertexSource, source.FragmentSource);
        // GLCall(glUseProgram(shader));

        Texture texture("res/textures/Picture.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.UnBind();
        vb.Unbind();
        ib.Unbind();
        shader.UnBind();
        */

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        //glm::vec3 translationA(200, 200, 0);
        //glm::vec3 translationB(400, 200, 0);

        //小动画参数
        //float r = 0.0f;
        //float increment = 0.05f;

        // 指针指向当前test
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;

        testMenu->RegisterTest<test::TestClearColor>("Clear Color");

        // test::TestClearColor test;

        while (!glfwWindowShouldClose(window))
        {
            
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();

            // test.OnUpdate(0.0f);
            // test.OnRender();

            ImGui_ImplGlfwGL3_NewFrame();

            if (currentTest)
            {
                currentTest->OnUpdate(0.0f);
                currentTest->OnRender();
                ImGui::Begin("Test");
                // 返回菜单
                if (currentTest != testMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->OnImGuiRender();

                ImGui::End();
            }

            // test.OnImGuiRender();

            /*
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                // 根据vertexArray与indexBuffer内容利用shader渲染图形
                renderer.Draw(va, ib, shader);

            }

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                // 根据vertexArray与indexBuffer内容利用shader渲染图形
                renderer.Draw(va, ib, shader);

            }

            // 任何索引缓冲都需要是无符号数
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            // 渐变动画
            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            // 渲染UI
            {
                // 连续一个float指针 指向translation的第一个位置 后续两个位置自动匹配
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            */

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        delete currentTest;
        if (currentTest != testMenu)
            delete testMenu;
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}