#pragma once
#include <Windows.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

#include <cstdint>

enum PixFormat
{
    I420,
    NV12,
    NV21
};

class OpenGLPlayer
{
public:
    OpenGLPlayer();
    ~OpenGLPlayer();

    void SetWindow(void *hwnd);
    void RenderFrame(uint8_t *data, uint32_t width, uint32_t height);

private:
    void Init();
    void InitContext();
    void DestroyContext();
    void InitShaders();
    void InitTexture();
    void CalcRenderPos();

private:
    uint32_t window_width_{};
    uint32_t window_height_{};
    HWND hwnd_{};
    HDC hdc_{};
    HGLRC hrc_{};

    GLuint texture_y_;
    GLuint texture_u_;
    GLuint texture_v_;
    GLuint uniform_y_;
    GLuint uniform_u_;
    GLuint uniform_v_;
    GLuint shader_program_;

    uint32_t render_width_{};
    uint32_t render_height_{};
};
