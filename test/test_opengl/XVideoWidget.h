#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QGLShaderProgram>
#include <mutex>
#include "depends.h"

struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();
    void Init(int width, int height);

    //不管成功与否都释放frame空间
    void Repaint(AVFrame *frame);

protected:  // 这三个函数是opengl需用用的
    // 刷新显示
    void paintGL() override;
    //初始化gl
    void initializeGL()override;
    // 窗口尺寸变化
    void resizeGL(int width, int height)override;

private:
    std::mutex mux;
    // shader 程序
    QGLShaderProgram program;
    // QOpenGLShaderProgram program;   // 下面这个是错误的

    //shader中yuv变量地址
    GLuint unis[3] = { 0 };
    //openg的 texture地址
    GLuint texs[3] = { 0 };

    //材质内存空间
    unsigned char *datas[3] = { 0 };

    int width = 240;
    int height = 128;
};
