#pragma once
#include <tchar.h>
#include <d3d9.h>

class Direct3D9Render
{
private:
public:
    Direct3D9Render();
    ~Direct3D9Render();
    int InitD3D(HWND hwnd, int lWidth, int lHeight);
    int drawVideo(const void *data, int pixel_w, int pixel_h);

private:
    void Cleanup();

private:
    CRITICAL_SECTION m_critial;
    IDirect3D9 *m_pDirect3D9 = NULL;
    IDirect3DDevice9 *m_pDirect3DDevice = NULL;
    IDirect3DSurface9 *m_pDirect3DSurfaceRender = NULL;

    RECT m_rtViewport;
};
