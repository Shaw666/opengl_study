
#include <windows.h>
#include <glad/glad.h>
#include "XOpenGlWidget.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QSurfaceFormat>

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
GLuint XOpenGlWidget::VBO;
GLuint XOpenGlWidget::VAO;
GLuint XOpenGlWidget::EBO;

//准备yuv数据
// ffmpeg -i v1080.mp4 -t 10 -s 240x128 -pix_fmt yuv420p  out240x128.yuv

XOpenGlWidget::XOpenGlWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	//不设置版本，默认2.0导致qpainter绘制崩溃，建议考虑全局设置
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	setFormat(format);
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
	memcpy(textures, tmp, sizeof(GLfloat) * 8);
	setVertices(1.0);
	QTimer *playerTimer = new QTimer(this);
	playerTimer->start(33);
	connect(playerTimer, &QTimer::timeout, this, [this]()
			{ update(); });
	QTimer * p1sTimer = new QTimer(this);
	connect(p1sTimer, &QTimer::timeout, this, [this]()
		{ time_str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"); });
	p1sTimer->start(1000);
	qDebug() << this->format().version();
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
					   if (!gladLoadGL())
					   {
						   qDebug() << "Failed to initialize GLAD";
						   return;
					   } });

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vString, NULL);
	glCompileShader(vs);

	glShaderSource(fs, 1, &tString, NULL);
	glCompileShader(fs);

	// #ifndef NDEBUG
	GLint success = 0;
	char infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				 << infoLog;
	}
	qDebug("vs::GL_COMPILE_STATUS=%d", success);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	qDebug("fs::GL_COMPILE_STATUS=%d", success);
	// #endif

	prog_yuv = glCreateProgram();

	glAttachShader(prog_yuv, vs);
	glAttachShader(prog_yuv, fs);

	glLinkProgram(prog_yuv);
	//#ifdef _DEBUG
	glGetProgramiv(prog_yuv, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(prog_yuv, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				 << infoLog;
	}
	qDebug("prog_yuv=%d GL_LINK_STATUS=%d", prog_yuv, success);
	//#endif
	glDeleteShader(vs);
	glDeleteShader(fs);
	qDebug("loadYUVShader ok");
	// glValidateProgram(prog_yuv);

	// opengl 3.3的纹理坐标是左上角0，0， 但是之前雷神博客说的是左下角0，0
	float vertices[] = {
		// positions          // texture coords
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// bottom right
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top right
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f	// top left
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3	 // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	qDebug("glVertexAttribPointer ok");

	texUniformY = glGetUniformLocation(prog_yuv, "tex_y");
	texUniformU = glGetUniformLocation(prog_yuv, "tex_u");
	texUniformV = glGetUniformLocation(prog_yuv, "tex_v");

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, y);
	// glTexImage2D 的 format 参数需要制定 data 的格式，如果你的图片是 RGB 三个通道，format 就是 GL_BGR （当然这里的通道顺序也和你加载图片相关），如果你的图片是 RGBA 四个通道，format 就是 GL_BGRA，需要你自己根据你的图片来决定这个 format 的。如果你的图片只有三个通道，但是你告诉 opengl 这是 GL_BGRA 的，那么它就会认为 data 中每 4 个byte 表示一个像素，当然就会发生越界的读取，而导致异常。
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(texUniformY, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_yuv[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w / 2, h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, u);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(texUniformU, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_yuv[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w / 2, h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, v);
	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(texUniformV, 2);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
}

void XOpenGlWidget::paintGL()
{
	qDebug() << "paintGL()";
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_listXVFrame.empty())
	{
		if(m_pFrame->data)
			drawYUV(m_pFrame);
	}
	else {
		m_pFrame = m_listXVFrame.front();
		m_listXVFrame.pop_front();
		drawYUV(m_pFrame);
	}
	glDisable(GL_DEPTH_TEST);
	drawText(QPoint(100, 100), time_str.toStdString().c_str(), 12, QColor(255, 0, 0));
	glEnable(GL_DEPTH_TEST);
}

void XOpenGlWidget::resizeGL(int width, int height)
{
	// mux.lock();
	qDebug() << "resizeGL()" << width << " : " << height;
	// mux.unlock();
}

#include <QPainter>
void XOpenGlWidget::drawText(QPoint lb, const char* text, int fontsize, QColor clr) {
	QPainter painter(this);
	QFont font = painter.font();
	font.setPixelSize(20);
	painter.setFont(font);
	painter.setPen(clr);
	painter.drawText(lb, text);
}
