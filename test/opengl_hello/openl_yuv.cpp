/**
 * 最简单的OpenGL播放视频的例子（OpenGL播放YUV）[Texture]
 * Simplest Video Play OpenGL (OpenGL play YUV) [Texture]
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序使用OpenGL播放YUV视频像素数据。本程序支持YUV420P的
 * 像素数据作为输入，经过转换后输出到屏幕上。其中用到了多种
 * 技术，例如Texture，Shader等，是一个相对比较复杂的例子。
 * 适合有一定OpenGL基础的初学者学习。
 *
 * 函数调用步骤如下:
 *
 * [初始化]
 * glutInit(): 初始化glut库。
 * glutInitDisplayMode(): 设置显示模式。
 * glutCreateWindow(): 创建一个窗口。
 * glewInit(): 初始化glew库。
 * glutDisplayFunc(): 设置绘图函数（重绘的时候调用）。
 * glutTimerFunc(): 设置定时器。
 * InitShaders(): 设置Shader。包含了一系列函数，暂不列出。
 * glutMainLoop(): 进入消息循环。
 *
 * [循环渲染数据]
 * glActiveTexture(): 激活纹理单位。
 * glBindTexture(): 绑定纹理
 * glTexImage2D(): 根据像素数据，生成一个2D纹理。
 * glUniform1i():
 * glDrawArrays(): 绘制。
 * glutSwapBuffers(): 显示。
 *
 * This software plays YUV raw video data using OpenGL.
 * It support read YUV420P raw file and show it on the screen.
 * It's use a slightly more complex technologies such as Texture,
 * Shaders etc. Suitable for beginner who already has some
 * knowledge about OpenGL.
 *
 * The process is shown as follows:
 *
 * [Init]
 * glutInit(): Init glut library.
 * glutInitDisplayMode(): Set display mode.
 * glutCreateWindow(): Create a window.
 * glewInit(): Init glew library.
 * glutDisplayFunc(): Set the display callback.
 * glutTimerFunc(): Set timer.
 * InitShaders(): Set Shader, Init Texture. It contains some functions about Shader.
 * glutMainLoop(): Start message loop.
 *
 * [Loop to Render data]
 * glActiveTexture(): Active a Texture unit
 * glBindTexture(): Bind Texture
 * glTexImage2D(): Specify pixel data to generate 2D Texture
 * glUniform1i():
 * glDrawArrays(): draw.
 * glutSwapBuffers(): show.
 */

#include <stdio.h>
#include <iostream>

// #include "glew.h"
// #include "glut.h"

// #include <Windows.h>
#define GLEW_STATIC
#include <GL/glew.h>

// #include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <string>
#include <thread>

GLenum glCheckError_(const char *file, int line)
{
    std::cout << "check error: " << file << " (" << line << ")" << std::endl;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Select one of the Texture mode (Set '1'):
#define TEXTURE_DEFAULT 0
// Rotate the texture
#define TEXTURE_ROTATE 0
// Show half of the Texture
#define TEXTURE_HALF 1
using namespace std;
const int screen_w = 800, screen_h = 400;
const int pixel_w = 1920, pixel_h = 1080;
// YUV file
FILE *infile = NULL;
unsigned char buf[pixel_w * pixel_h * 3 / 2];
unsigned char *plane[3];

GLuint p;
GLuint id_y, id_u, id_v; // Texture id
GLuint textureUniformY, textureUniformU, textureUniformV;

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

GLFWwindow *window;

GLenum err;

    unsigned int VBO, VAO, EBO;

void display(void)
{
    // 4/6 2/6
    unsigned int len = fread(buf, 1, pixel_w * pixel_h * 3 / 2, infile);
    cout << "read file " << len << endl;
    if (len != pixel_w * pixel_h * 3 / 2)
    {
        cout << "read failed " << len << endl;
        // Loop
        fseek(infile, 0, SEEK_SET);
        fread(buf, 1, pixel_w * pixel_h * 3 / 2, infile);
    }
    // Clear
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // Y
    //
    glShadeModel(GL_FLAT);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w, pixel_h, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);
    glUniform1i(textureUniformY, 0);
    // U
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);
    glUniform1i(textureUniformU, 1);
    // V
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);
    glUniform1i(textureUniformV, 2);

    // Draw
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // Show
    // Double
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    // Single
    // glFlush();
    GLenum err = glGetError();
    fprintf(stderr, "gl error : %d \n", err);
    // glfwSwapBuffers(window);
    // SwapBuffers(window);
}

// void timeFunc(int value)
// {
//     display();
//     // Timer: 40ms
//     glfwTimerFunc(40, timeFunc, 0);
// }

// char *textFileRead(char *filename)
// {
//     char *s = (char *)malloc(8000);
//     memset(s, 0, 8000);
//     FILE *infile = fopen(filename, "rb");
//     int len = fread(s, 1, 8000, infile);
//     fclose(infile);
//     s[len] = 0;
//     return s;
// }
// 自动加引号   b站的源码
#define GET_STR(x) #x
// Init Shader

void InitShaders()
{
    float vertices[] = {
        0.5,
        0.5,
        0.0,
        1.0,
        0.0,
        0.5,
        -0.5,
        0.0,
        1.0,
        1.0,
        -0.5,
        -0.5,
        0.0,
        0.0,
        1.0,
        -0.5,
        0.5,
        0.0,
        0.0,
        0.0,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};
    // clang-format on



    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    err = glCheckError();

    std::string vsh_str =
        "attribute vec4 vertexIn;\n"
        "attribute vec2 textureIn;\n"
        "varying vec2 textureOut;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = vertexIn;\n"
        "    textureOut = textureIn;\n"
        "}\n";

    std::string fsh_str =
        "varying vec2 textureOut;\n"
        "uniform sampler2D tex_y;\n"
        "uniform sampler2D tex_u;\n"
        "uniform sampler2D tex_v;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 yuv;\n"
        "    vec3 rgb;\n"
        "    yuv.x = texture2D(tex_y, textureOut).r;\n"
        "    yuv.y = texture2D(tex_u, textureOut).r - 0.5;\n"
        "    yuv.z = texture2D(tex_v, textureOut).r - 0.5;\n"
        "    rgb = mat3( 1,       1,         1,\n"
        "                0,       -0.39465,  2.03211,\n"
        "                1.13983, -0.58060,  0) * yuv;\n"
        "    gl_FragColor = vec4(rgb, 1);\n"
        "}\n";

    GLint vertCompiled, fragCompiled, linked;

    GLint v, f;
    const char *vs, *fs;
    // Shader: step1
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    // Get source code
    vs = vsh_str.c_str();
    fs = fsh_str.c_str();
    // Shader: step2
    glShaderSource(v, 1, &vs, NULL);
    glShaderSource(f, 1, &fs, NULL);
    // Shader: step3
    glCompileShader(v);
    // Debug
    glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);
    printf("v complile flag: %d\n", vertCompiled);

    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);
    printf("f complile flag: %d\n", fragCompiled);
    // Program: Step1
    p = glCreateProgram();
    // Program: Step2
    glAttachShader(p, v);
    glAttachShader(p, f);

    glBindAttribLocation(p, ATTRIB_VERTEX, "vertexIn");
    glBindAttribLocation(p, ATTRIB_TEXTURE, "textureIn");
    // Program: Step3
    glLinkProgram(p);
    // Debug
    glGetProgramiv(p, GL_LINK_STATUS, &linked);
    printf("link flag: %d\n", fragCompiled);
    // Program: Step4
    glUseProgram(p);
    err = glCheckError();

    glDeleteShader(v);
    glDeleteShader(f);
    err = glCheckError();
    // #if TEXTURE_ROTATE
    //     static const GLfloat vertexVertices[] = {
    //         -1.0f,
    //         -0.5f,
    //         0.5f,
    //         -1.0f,
    //         -0.5f,
    //         1.0f,
    //         1.0f,
    //         0.5f,
    //     };
    // #else
    //     static const GLfloat vertexVertices[] = {
    //         -1.0f,
    //         -1.0f,
    //         1.0f,
    //         -1.0f,
    //         -1.0f,
    //         1.0f,
    //         1.0f,
    //         1.0f,
    //     };
    // #endif

    //     // #if TEXTURE_HALF
    //     //     static const GLfloat textureVertices[] = {
    //     //         0.0f,
    //     //         1.0f,
    //     //         0.5f,
    //     //         1.0f,
    //     //         0.0f,
    //     //         0.0f,
    //     //         0.5f,
    //     //         0.0f,
    //     //     };
    //     // #else
    //     static const GLfloat textureVertices[] = {
    //         0.0f,
    //         1.0f,
    //         1.0f,
    //         1.0f,
    //         0.0f,
    //         0.0f,
    //         1.0f,
    //         0.0f,
    //     };
    //     // #endif
    //     // Set Arrays

    // Get Uniform Variables Location
    textureUniformY = glGetUniformLocation(p, "tex_y");
    textureUniformU = glGetUniformLocation(p, "tex_u");
    textureUniformV = glGetUniformLocation(p, "tex_v");
    // Init Texture
    glGenTextures(1, &id_y);
    glBindTexture(GL_TEXTURE_2D, id_y);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &id_u);
    glBindTexture(GL_TEXTURE_2D, id_u);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &id_v);
    glBindTexture(GL_TEXTURE_2D, id_v);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    err = glCheckError();
}

//窗口大小变化时，重新设置视口
void framebuff_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//处理输入
void process_input_callabck(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char *argv[])
{
    // Open YUV420P file
    if ((infile = fopen("../../test_1920x1080420p.yuv", "rb")) == NULL)
    {
        printf("cannot open this file\n");
        return -1;
    }

    // YUV Data
    plane[0] = buf;
    plane[1] = plane[0] + pixel_w * pixel_h;
    plane[2] = plane[1] + pixel_w * pixel_h / 4;

    if (!glfwInit())
    {
        printf("Couldn't init GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(screen_w, screen_h, "Hello World", NULL, NULL);
    if (!window)
    {
        printf("Couldn't open window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, process_input_callabck);
    glfwSetFramebufferSizeCallback(window, framebuff_size_callback);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }
    err = glGetError();
    fprintf(stderr, "gl error : %d \n", err);
    InitShaders();
    err = glCheckError();
    if (err != GL_NO_ERROR)
        return -1;
    while (1)
    {
        display();
        //检查有没有触发什么事件（键盘输入、鼠标移动等)、窗口改变
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(66));
    }

    return 0;
}