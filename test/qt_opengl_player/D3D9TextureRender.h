#pragma once
#include <d3d9.h>
//渲染rgb

class D3D9TextureRender
{
private:
public:
    D3D9TextureRender();
    ~D3D9TextureRender();
    int InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight);
    bool drawVideo(const void*, int pixel_w, int pixel_h);

private:
    void Cleanup();

private:
    IDirect3D9 *m_pDirect3D9 = NULL;
    IDirect3DDevice9 *m_pDirect3DDevice = NULL;
    IDirect3DTexture9 *m_pDirect3DTexture = NULL;
    IDirect3DVertexBuffer9 *m_pDirect3DVertexBuffer = NULL;
    CRITICAL_SECTION m_critial;
    HWND m_hVideoWnd; // 视频窗口
};
