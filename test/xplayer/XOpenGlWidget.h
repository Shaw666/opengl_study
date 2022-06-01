#pragma once
#define GLEW_STATIC
#include "GL/glew.h"
#include <QOpenGLWidget>
// #include <QOpenGLFunctions>
// #include <QOpenGLShaderProgram>
// #include <QGLShaderProgram>
#include <mutex>

constexpr auto kFrameWidth = 1920;
constexpr auto kFrameHeight = 1080;
constexpr auto kFrameSize = kFrameWidth * kFrameHeight * 3 / 2;

struct XVFrame
{
    int w;
    int h;
    int type;
    long long ts;
    unsigned char *data;
    unsigned int size;
    XVFrame(unsigned char* data, unsigned int size, int w = kFrameWidth, int h = kFrameHeight, int type = 0, long long ts = 0) :w(w), h(h), type(type), ts(ts), data(data), size(size) {};
};

class XOpenGlWidget : public QOpenGLWidget
// , protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XOpenGlWidget(QWidget *parent);
    ~XOpenGlWidget();
    void Init(int width, int height);
    void setAspectRatio(double ratio);
    //不管成功与否都释放frame空间
    void drawYUV(XVFrame *frame);
    void pushFrame(char *data, unsigned int size)
    {
        m_listXVFrame.push_back(new XVFrame(reinterpret_cast<unsigned char*>(data), size));
    };

protected: // 这三个函数是opengl需用用的
    // 刷新显示
    void paintGL() override;
    //初始化gl
    void initializeGL() override;
    // 窗口尺寸变化
    void resizeGL(int width, int height) override;

private:
    void setVertices(double ratio);
    void setVertices(QRect rc);

private:
    static GLuint prog_yuv;
    static GLuint texUniformY;
    static GLuint texUniformU;
    static GLuint texUniformV;
    GLuint tex_yuv[3];

    double aspect_ratio;
    GLfloat vertices[8];
    GLfloat textures[8];

    // NOTE: QPainter used 3 VertexAttribArray
    enum VER_ATTR
    {
        VER_ATTR_VER = 3,
        VER_ATTR_TEX,
    };

    std::list<XVFrame *> m_listXVFrame;
    // std::mutex mux;
    // // shader 程序
    // QGLShaderProgram program;
    // // QOpenGLShaderProgram program;   // 下面这个是错误的
    // // shader中yuv变量地址
    // GLuint unis[3] = {0};
    // // openg的 texture地址
    // GLuint texs[3] = {0};
    // //材质内存空间
    // unsigned char *datas[3] = {0};
};
