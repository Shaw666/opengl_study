#include "XOpenGlWidget.h"
#include <QDebug>
#include <QTimer>

#define A_VER 3
#define T_VER 4

// 自动加引号   b站的源码
#define GET_STR(x) #x

FILE *fp = nullptr;

//顶点shader
const char *vString = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void) {
		// GL_POSITION = vertexIn;
		gl_Position = vertexIn;
		textureOut = textureIn;
	});

//片元shader
const char *tString = GET_STR(
	varying vec2 textureOut;
	uniform sampler2D tex_y;
	uniform sampler2D tex_u;
	uniform sampler2D tex_v;
	void main(void) {
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(tex_y, textureOut).r;
		yuv.y = texture2D(tex_u, textureOut).r - 0.5;
		yuv.z = texture2D(tex_v, textureOut).r - 0.5;
		rgb = mat3(1.0, 1.0, 1.0,
				   0.0, -0.39465, 2.03211,
				   1.13983, -0.58060, 0.0) *
			  yuv;
		gl_FragColor = vec4(rgb, 1.0);
	});

GLuint XOpenGlWidget::prog_yuv;
GLuint XOpenGlWidget::texUniformY;
GLuint XOpenGlWidget::texUniformU;
GLuint XOpenGlWidget::texUniformV;

//准备yuv数据
// ffmpeg -i v1080.mp4 -t 10 -s 240x128 -pix_fmt yuv420p  out240x128.yuv

XOpenGlWidget::XOpenGlWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	aspect_ratio = 0.0;
	GLfloat tmp[] = {
		0.0f,
		1.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
	};

	// reverse
	/*
	GLfloat tmp[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};
	*/
	memcpy(textures, tmp, sizeof(GLfloat) * 8);
	setVertices(1.0);
	QTimer *playerTimer = new QTimer(this);
	playerTimer->start(33);
	connect(playerTimer, &QTimer::timeout, this, [this]()
			{ update(); });
}

XOpenGlWidget::~XOpenGlWidget()
{
}

void XOpenGlWidget::setAspectRatio(double ratio)
{
	aspect_ratio = ratio;
	if (aspect_ratio == 0)
	{
		setVertices(1.0);
	}
	else
	{
		setVertices((double)height() / (double)width() * aspect_ratio);
	}
}

void XOpenGlWidget::setVertices(double ratio)
{
	GLfloat w = 1.0, h = 1.0;
	if (ratio < 1.0)
	{
		w = ratio;
	}
	else
	{
		h = 1.0 / ratio;
	}

	GLfloat tmp[] = {
		-w,
		-h,
		w,
		-h,
		-w,
		h,
		w,
		h,
	};

	memcpy(vertices, tmp, sizeof(GLfloat) * 8);
}

void XOpenGlWidget::setVertices(QRect rc)
{
	int wnd_w = width();
	int wnd_h = height();
	if (wnd_w <= 0 || wnd_h <= 0)
	{
		return;
	}
	GLfloat left = (GLfloat)rc.left() * 2 / wnd_w - 1;
	GLfloat right = (GLfloat)(rc.right() + 1) * 2 / wnd_w - 1;
	GLfloat top = 1 - (GLfloat)rc.top() * 2 / wnd_h;
	GLfloat bottom = 1 - (GLfloat)(rc.bottom() + 1) * 2 / wnd_h;
	qDebug("l=%d r=%d t=%d b=%d", rc.left(), rc.right(), rc.top(), rc.bottom());
	qDebug("l=%f r=%f t=%f b=%f", left, right, top, bottom);
	GLfloat tmp[] = {
		left, bottom,
		right, bottom,
		left, top,
		right, top};

	memcpy(vertices, tmp, sizeof(GLfloat) * 8);
}

// void XOpenGlWidget::Repaint(AVFrame *frame)
// {
// 	if (!frame)
// 		return;
// 	// mux.lock();
// 	//容错，保证尺寸正确
// 	if (!datas[0] || width * height == 0 || frame->width != this->width || frame->height != this->height)
// 	{
// 		// av_frame_free(&frame);
// 		// mux.unlock();
// 		return;
// 	}
// 	if (width == frame->linesize[0]) // 不需要对齐
// 	{
// 		memcpy(datas[0], frame->data[0], width * height);
// 		memcpy(datas[1], frame->data[1], width * height / 4);
// 		memcpy(datas[2], frame->data[2], width * height / 4);
// 	}
// 	else //行对齐问题
// 	{
// 		for (int i = 0; i < height; i++) // Y
// 			memcpy(datas[0] + width * i, frame->data[0] + frame->linesize[0] * i, width);
// 		for (int i = 0; i < height / 2; i++) // U
// 			memcpy(datas[1] + width / 2 * i, frame->data[1] + frame->linesize[1] * i, width);
// 		for (int i = 0; i < height / 2; i++) // V
// 			memcpy(datas[2] + width / 2 * i, frame->data[2] + frame->linesize[2] * i, width);
// 	}

// 	// mux.unlock();
// 	//  av_frame_free(&frame);
// 	//刷新显示
// 	update();
// }

void XOpenGlWidget::initializeGL()
{
	qDebug() << "initializeGL()";
	static std::once_flag flag;
	std::call_once(flag, []()
				   {
		if (glewInit() != 0) 
		{
			qFatal("glewInit failed");
			return;
        } });

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vString, NULL);
	glCompileShader(vs);

	glShaderSource(fs, 1, &tString, NULL);
	glCompileShader(fs);

#ifndef NDEBUG
	GLint iRet = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &iRet);
	qDebug("vs::GL_COMPILE_STATUS=%d", iRet);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &iRet);
	qDebug("fs::GL_COMPILE_STATUS=%d", iRet);
#endif

	prog_yuv = glCreateProgram();

	glAttachShader(prog_yuv, vs);
	glAttachShader(prog_yuv, fs);

	glBindAttribLocation(prog_yuv, VER_ATTR_VER, "verIn");
	glBindAttribLocation(prog_yuv, VER_ATTR_TEX, "texIn");

	glLinkProgram(prog_yuv);

	//#ifdef _DEBUG
	glGetProgramiv(prog_yuv, GL_LINK_STATUS, &iRet);
	qDebug("prog_yuv=%d GL_LINK_STATUS=%d", prog_yuv, iRet);
	//#endif

	glValidateProgram(prog_yuv);

	texUniformY = glGetUniformLocation(prog_yuv, "tex_y");
	texUniformU = glGetUniformLocation(prog_yuv, "tex_u");
	texUniformV = glGetUniformLocation(prog_yuv, "tex_v");

	qDebug("loadYUVShader ok");

	glVertexAttribPointer(VER_ATTR_VER, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(VER_ATTR_VER);

	glVertexAttribPointer(VER_ATTR_TEX, 2, GL_FLOAT, GL_FALSE, 0, textures);
	glEnableVertexAttribArray(VER_ATTR_TEX);

	qDebug("glVertexAttribPointer ok");

	glGenTextures(3, tex_yuv);
	for (int i = 0; i < 3; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, tex_yuv[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	qDebug("inti yuv Texture ok");

	// // mux.lock();
	// //  初始化opengl (QOpenGLFunctions继承)函数
	// initializeOpenGLFunctions(); // 这个必须加上去

	// // program 加在shader （顶点和片元）脚本
	// // 片元（像素）
	// qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString); // QGLShader::Fragment

	// // 顶点shader
	// qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);

	// //设置顶点坐标的变量
	// program.bindAttributeLocation("vertexIn", A_VER);

	// // 设置材质坐标
	// program.bindAttributeLocation("textureIn", T_VER);

	// // 编译shader
	// qDebug() << "program.link() = " << program.link();

	// qDebug() << "program.bind() = " << program.bind();

	// //传递顶点和材质坐标
	// //顶点
	// static const GLfloat ver[] = {
	// 	-1.0f, -1.0f,
	// 	1.0f, -1.0f,
	// 	-1.0f, 1.0f,
	// 	1.0f, 1.0f};

	// //材质
	// static const GLfloat tex[] = {
	// 	0.0f, 1.0f,
	// 	1.0f, 1.0f,
	// 	0.0f, 0.0f,
	// 	1.0f, 0.0f};

	// //顶点
	// glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
	// glEnableVertexAttribArray(A_VER);

	// //材质
	// glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	// glEnableVertexAttribArray(T_VER);

	// //从shader获取材质
	// unis[0] = program.uniformLocation("tex_y");
	// unis[1] = program.uniformLocation("tex_u");
	// unis[2] = program.uniformLocation("tex_v");

	// // mux.unlock();
	// // mux.lock();
	// this->width = width;
	// this->height = height;
	// delete datas[0];
	// delete datas[1];
	// delete datas[2];

	// ///分配材质内存空间
	// datas[0] = new unsigned char[width * height];	  // Y
	// datas[1] = new unsigned char[width * height / 4]; // U
	// datas[2] = new unsigned char[width * height / 4]; // V

	// if (texs[0])
	// {
	// 	glDeleteTextures(3, texs); // 释放空间
	// }

	// //创建材质
	// glGenTextures(3, texs);

	// // Y
	// glBindTexture(GL_TEXTURE_2D, texs[0]);
	// //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)  GL_LINEAR
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// //创建材质显卡空间
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// // U
	// glBindTexture(GL_TEXTURE_2D, texs[1]);
	// //放大过滤，线性插值
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// //创建材质显卡空间
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// // V
	// glBindTexture(GL_TEXTURE_2D, texs[2]);
	// //放大过滤，线性插值
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// //创建材质显卡空间
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// // mux.unlock();
}

void XOpenGlWidget::drawYUV(XVFrame *pFrame)
{
	// assert(pFrame->type == PIX_FMT_IYUV || pFrame->type == PIX_FMT_YV12);
	qDebug() << pFrame->size;
	int w = pFrame->w;
	int h = pFrame->h;
	int y_size = w * h;
	GLubyte *y = (GLubyte *)pFrame->data;
	GLubyte *u = y + y_size;
	GLubyte *v = u + (y_size >> 2);
	// if (pFrame->type == PIX_FMT_YV12)
	// {
	// 	GLubyte *tmp = u;
	// 	u = v;
	// 	v = tmp;
	// }

	glUseProgram(prog_yuv);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_yuv[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, y);
	glUniform1i(texUniformY, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_yuv[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w / 2, h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u);
	glUniform1i(texUniformU, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_yuv[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w / 2, h / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v);
	glUniform1i(texUniformV, 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);
}

void XOpenGlWidget::paintGL()
{
	qDebug() << "paintGL()";
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (m_listXVFrame.empty())
		return;
	drawYUV(m_listXVFrame.front());
	m_listXVFrame.pop_front();
	// // if (feof(fp))
	// //{
	// //	fseek(fp, 0, SEEK_SET);
	// // }
	// // fread(datas[0], 1, width * height, fp);
	// // fread(datas[1], 1, width * height / 4, fp);
	// // fread(datas[2], 1, width * height / 4, fp);
	// // mux.lock();

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, texs[0]); // 0层绑定到Y材质
	// //修改材质内容(复制内存内容)
	// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	// //与shader uni遍历关联
	// glUniform1i(unis[0], 0);

	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_2D, texs[1]); // 1层绑定到U材质
	// 									   //修改材质内容(复制内存内容)
	// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	// //与shader uni遍历关联
	// glUniform1i(unis[1], 1);

	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_2D, texs[2]); // 2层绑定到V材质
	// 									   //修改材质内容(复制内存内容)
	// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	// //与shader uni遍历关联
	// glUniform1i(unis[2], 2);

	// glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// // mux.unlock();
}

void XOpenGlWidget::resizeGL(int width, int height)
{
	// mux.lock();
	qDebug() << "resizeGL()" << width << " : " << height;
	// mux.unlock();
}
