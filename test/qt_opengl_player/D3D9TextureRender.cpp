#include "D3D9TextureRender.h"
//渲染rgb

//Flexible Vertex Format, FVF
typedef struct
{
	FLOAT x, y, z;
	FLOAT rhw;
	D3DCOLOR diffuse;
	FLOAT tu, tv;
} CUSTOMVERTEX;

// Custom flexible vertex format (FVF), which describes custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//Select one of the Texture mode (Set '1'):
#define TEXTURE_DEFAULT 1
//Rotate the texture
#define TEXTURE_ROTATE 0
//Show half of the Texture
#define TEXTURE_HALF 0

//Width, Height
// const int screen_w=500,screen_h=500;
// const int pixel_w=320,pixel_h=180;
// FILE *fp=NULL;
//Bit per Pixel
const int bpp = 32;

// unsigned char buffer[pixel_w*pixel_h*bpp/8];

D3D9TextureRender::D3D9TextureRender()
{
}

D3D9TextureRender::~D3D9TextureRender()
{
	Cleanup();
}

void D3D9TextureRender::Cleanup()
{
	EnterCriticalSection(&m_critial);
	if (m_pDirect3DVertexBuffer)
		m_pDirect3DVertexBuffer->Release();
	if (m_pDirect3DTexture)
		m_pDirect3DTexture->Release();
	if (m_pDirect3DDevice)
		m_pDirect3DDevice->Release();
	if (m_pDirect3D9)
		m_pDirect3D9->Release();
	LeaveCriticalSection(&m_critial);
}

int D3D9TextureRender::InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight)
{
	HRESULT lRet;
	InitializeCriticalSection(&m_critial);

	Cleanup();

	EnterCriticalSection(&m_critial);
	// Create IDirect3D
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
	{
		LeaveCriticalSection(&m_critial);
		return -1;
	}

	if (lWidth == 0 || lHeight == 0)
	{
		RECT rt;
		GetClientRect(hwnd, &rt);
		lWidth = rt.right - rt.left;
		lHeight = rt.bottom - rt.top;
	}

	/*
	//Get Some Info
	//Retrieves device-specific information about a device.
	D3DCAPS9 d3dcaps;
	lRet=m_pDirect3D9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&d3dcaps);
	int hal_vp = 0;
	if( d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ){
		//save in hal_vp the fact that hardware vertex processing is supported.
		hal_vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	// get D3DDISPLAYMODE
	D3DDISPLAYMODE d3dDisplayMode;
	lRet = m_pDirect3D9->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3dDisplayMode );
	if ( FAILED(lRet) ){
		LeaveCriticalSection(&m_critial);
		return -1;
	}
	*/

	//D3DPRESENT_PARAMETERS Describes the presentation parameters.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = lWidth;
	d3dpp.BackBufferHeight = lHeight;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	m_hVideoWnd = hwnd;

	//Creates a device to represent the display adapter.
	//Adapter:		Ordinal number that denotes the display adapter. D3DADAPTER_DEFAULT is always the primary display
	//D3DDEVTYPE:	D3DDEVTYPE_HAL((Hardware Accelerator), or D3DDEVTYPE_SW(SoftWare)
	//BehaviorFlags：D3DCREATE_SOFTWARE_VERTEXPROCESSING, or D3DCREATE_HARDWARE_VERTEXPROCESSING
	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL,
									  D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &m_pDirect3DDevice);

	/*
	//Set some property
	//SetSamplerState()
	// Texture coordinates outside the range [0.0, 1.0] are set
	// to the texture color at 0.0 or 1.0, respectively.
	IDirect3DDevice9_SetSamplerState(m_pDirect3DDevice, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	IDirect3DDevice9_SetSamplerState(m_pDirect3DDevice, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	// Set linear filtering quality
	IDirect3DDevice9_SetSamplerState(m_pDirect3DDevice, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	IDirect3DDevice9_SetSamplerState(m_pDirect3DDevice, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//SetRenderState()
	//set maximum ambient light
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_AMBIENT, D3DCOLOR_XRGB(255,255,0));
	// Turn off culling
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_CULLMODE, D3DCULL_NONE);
	// Turn off the zbuffer
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_ZENABLE, D3DZB_FALSE);
	// Turn off lights
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_LIGHTING, FALSE);
	// Enable dithering
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_DITHERENABLE, TRUE);
	// disable stencil
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_STENCILENABLE, FALSE);
	// manage blending
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_ALPHABLENDENABLE, TRUE);
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_ALPHATESTENABLE,TRUE);
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_ALPHAREF, 0x10);
	IDirect3DDevice9_SetRenderState(m_pDirect3DDevice, D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	// Set texture states
	IDirect3DDevice9_SetTextureStageState(m_pDirect3DDevice, 0, D3DTSS_COLOROP,D3DTOP_MODULATE);
	IDirect3DDevice9_SetTextureStageState(m_pDirect3DDevice, 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
	IDirect3DDevice9_SetTextureStageState(m_pDirect3DDevice, 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	// turn off alpha operation
	IDirect3DDevice9_SetTextureStageState(m_pDirect3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	*/

	//Creates a texture resource.
	//Usage:
	//D3DUSAGE_SOFTWAREPROCESSING: If this flag is used, vertex processing is done in software.
	//							If this flag is not used, vertex processing is done in hardware.
	//D3DPool:
	//D3D3POOL_DEFAULT:	Resources are placed in the hardware memory (Such as video memory)
	//D3D3POOL_MANAGED:	Resources are placed automatically to device-accessible memory as needed.
	//D3DPOOL_SYSTEMMEM:  Resources are placed in system memory.

	lRet = m_pDirect3DDevice->CreateTexture(lWidth, lHeight, 1, D3DUSAGE_SOFTWAREPROCESSING,
											D3DFMT_X8R8G8B8,
											D3DPOOL_MANAGED,
											&m_pDirect3DTexture, NULL);

	if (FAILED(lRet))
	{
		LeaveCriticalSection(&m_critial);
		return -1;
	}
	// Create Vertex Buffer
	lRet = m_pDirect3DDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
												 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pDirect3DVertexBuffer, NULL);
	if (FAILED(lRet))
	{
		LeaveCriticalSection(&m_critial);
		return -1;
	}

#if TEXTURE_HALF
	CUSTOMVERTEX vertices[] = {
		{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.0f},
		{lWidth, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.5f, 0.0f},
		{lWidth, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.5f, 1.0f},
		{0.0f, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f}};
#elif TEXTURE_ROTATE
	//Rotate Texture?
	CUSTOMVERTEX vertices[] = {
		{lWidth / 4, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.0f},
		{lWidth, lHeight / 4, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0.0f},
		{lWidth * 3 / 4, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 1.0f},
		{0.0f, lHeight * 3 / 4, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f}};
#else
	CUSTOMVERTEX vertices[] = {
		{0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.0f},
		{lWidth, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0.0f},
		{lWidth, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 1.0f},
		{0.0f, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f}};
#endif

	// Fill Vertex Buffer
	CUSTOMVERTEX *pVertex;
	lRet = m_pDirect3DVertexBuffer->Lock(0, 4 * sizeof(CUSTOMVERTEX), (void **)&pVertex, 0);
	if (FAILED(lRet))
	{
		LeaveCriticalSection(&m_critial);
		return -1;
	}
	memcpy(pVertex, vertices, sizeof(vertices));

	m_pDirect3DVertexBuffer->Unlock();
	LeaveCriticalSection(&m_critial);
	return 0;
}

bool D3D9TextureRender::drawVideo(const void *data, int pixel_w, int pixel_h)
{
	LRESULT lRet;
	if (data == NULL || m_pDirect3DDevice == NULL)
		return false;
	//Clears one or more surfaces
	lRet = m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
									D3DCOLOR_XRGB(0, 255, 0), 1.0f, 0);

	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DTexture->LockRect(0, &d3d_rect, 0, 0);
	if (FAILED(lRet))
	{
		return false;
	}
	byte *pSrc = (byte *)data;
	byte *pDest = (byte *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	for (int i = 0; i < pixel_h; i++)
	{
		memcpy(pDest + i * stride, pSrc + i * pixel_w, pixel_w);
	}
	m_pDirect3DTexture->UnlockRect(0);

	D3DLOCKED_RECT d3d_rect1;
	lRet = m_pDirect3DTexture->LockRect(0, &d3d_rect1, 0, 0);
	if (FAILED(lRet))
	{
		return false;
	}
	pSrc = (byte *)data + pixel_w * pixel_h;
	pDest = (byte *)d3d_rect1.pBits;
	stride = d3d_rect1.Pitch;
	for (int i = 0; i < pixel_h/2; i++)
	{
		memcpy(pDest + i * stride, pSrc + i * pixel_w, pixel_w);
	}
	m_pDirect3DTexture->UnlockRect(0);

	D3DLOCKED_RECT d3d_rect2;
	lRet = m_pDirect3DTexture->LockRect(0, &d3d_rect2, 0, 0);
	if (FAILED(lRet))
	{
		return false;
	}
	pSrc = (byte *)data + pixel_w * pixel_h;
	pDest = (byte *)d3d_rect2.pBits;
	stride = d3d_rect2.Pitch;
	for (int i = 0; i < pixel_h/2; i++)
	{
		memcpy(pDest + i * stride, pSrc + i * pixel_w, pixel_w);
	}
	m_pDirect3DTexture->UnlockRect(0);




	//Begin the scene
	if (FAILED(m_pDirect3DDevice->BeginScene()))
	{
		return false;
	}
	lRet = m_pDirect3DDevice->SetTexture(0, m_pDirect3DTexture);
	//Binds a vertex buffer to a device data stream.
	m_pDirect3DDevice->SetStreamSource(0, m_pDirect3DVertexBuffer,
									   0, sizeof(CUSTOMVERTEX));
	//Sets the current vertex stream declaration.
	lRet = m_pDirect3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	//Renders a sequence of nonindexed, geometric primitives of the
	//specified type from the current set of data input streams.
	m_pDirect3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	m_pDirect3DDevice->EndScene();
	//Presents the contents of the next buffer in the sequence of back
	//buffers owned by the device.
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	return true;
}

//渲染rgb
// bool D3D9TextureRender::drawVideo(const void* data, int pixel_w, int pixel_h)
// {
// 	LRESULT lRet;
// 	//Read Data
// 	//RGB
// 	// if (fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp) != pixel_w*pixel_h*bpp/8){
// 	// 	// Loop
// 	// 	fseek(fp, 0, SEEK_SET);
// 	// 	fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp);
// 	// }

// 	if(data == NULL || m_pDirect3DDevice == NULL)
// 		return false;
// 	//Clears one or more surfaces
// 	lRet = m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
// 		D3DCOLOR_XRGB(0, 255, 0), 1.0f, 0);

// 	D3DLOCKED_RECT d3d_rect;
// 	//Locks a rectangle on a texture resource.
// 	//And then we can manipulate pixel data in it.
// 	lRet = m_pDirect3DTexture->LockRect( 0, &d3d_rect, 0, 0 );
// 	if ( FAILED(lRet) ){
// 		return false;
// 	}
// 	// Copy pixel data to texture
// 	byte *pSrc = (byte*)data;
// 	byte *pDest = (byte *)d3d_rect.pBits;
// 	int stride = d3d_rect.Pitch;

// 	int pixel_w_size=pixel_w*bpp/8;
// 	for(int i=0; i< pixel_h; i++){
// 		memcpy( pDest, pSrc, pixel_w_size );
// 		pDest += stride;
// 		pSrc += pixel_w_size;
// 	}

// 	m_pDirect3DTexture->UnlockRect( 0 );

// 	//Begin the scene
// 	if ( FAILED(m_pDirect3DDevice->BeginScene()) ){
// 		return false;
// 	}

// 	lRet = m_pDirect3DDevice->SetTexture( 0, m_pDirect3DTexture );

// 	//Binds a vertex buffer to a device data stream.
// 	m_pDirect3DDevice->SetStreamSource( 0, m_pDirect3DVertexBuffer,
// 		0, sizeof(CUSTOMVERTEX) );
// 	//Sets the current vertex stream declaration.
// 	lRet = m_pDirect3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
// 	//Renders a sequence of nonindexed, geometric primitives of the
// 	//specified type from the current set of data input streams.
// 	m_pDirect3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
// 	m_pDirect3DDevice->EndScene();
// 	//Presents the contents of the next buffer in the sequence of back
// 	//buffers owned by the device.
// 	m_pDirect3DDevice->Present( NULL, NULL, NULL, NULL );
// 	return true;
// }

// LRESULT WINAPI MyWndProc(HWND hwnd, UINT msg, WPARAM wparma, LPARAM lparam)
// {
// 	switch(msg){
// 	case WM_DESTROY:
// 		Cleanup();
// 		PostQuitMessage(0);
// 		return 0;
// 	}
// 	return DefWindowProc(hwnd, msg, wparma, lparam);
// }

// int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
// {
// 	WNDCLASSEX wc;
// 	ZeroMemory(&wc, sizeof(wc));

// 	wc.cbSize = sizeof(wc);
// 	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
// 	wc.lpfnWndProc = (WNDPROC)MyWndProc;
// 	wc.lpszClassName = L"D3D";
// 	wc.style = CS_HREDRAW | CS_VREDRAW;

// 	RegisterClassEx(&wc);

// 	HWND hwnd = NULL;
// 	hwnd = CreateWindow(L"D3D", L"Simplest Video Play Direct3D (Texture)", WS_OVERLAPPEDWINDOW, 100, 100, screen_w, screen_h, NULL, NULL, hInstance, NULL);
// 	if (hwnd==NULL){
// 		return -1;
// 	}

// 	if(InitD3D( hwnd, pixel_w, pixel_h)==E_FAIL){
// 		return -1;
// 	}

// 	ShowWindow(hwnd, nShowCmd);
// 	UpdateWindow(hwnd);

// 	fp=fopen("../test_bgra_320x180.rgb","rb+");

// 	if(fp==NULL){
// 		printf("Cannot open this file.\n");
// 		return -1;
// 	}

// 	MSG msg;
// 	ZeroMemory(&msg, sizeof(msg));

// 	while (msg.message != WM_QUIT){
// 		//PeekMessage, not GetMessage
// 		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 		else{
// 			Sleep(40);
// 			Render();
// 		}
// 	}

// 	UnregisterClass(L"D3D", hInstance);
// 	return 0;
// }
