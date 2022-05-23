//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: ps_multitex.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Deomstrates multi-texturing using a pixel shader.  You will have
//       to switch to the REF device to run this sample if your hardware
//       doesn't support pixel shaders.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 320;
const int Height = 180;

IDirect3DPixelShader9* MultiTexPS = 0;
ID3DXConstantTable* MultiTexCT    = 0;

IDirect3DVertexBuffer9* QuadVB = 0;

IDirect3DTexture9* YTex      = 0;
IDirect3DTexture9* UTex = 0;
IDirect3DTexture9* VTex    = 0;

D3DXHANDLE YTexHandle      = 0;
D3DXHANDLE UTexHandle = 0;
D3DXHANDLE VTexHandle    = 0;

D3DXCONSTANT_DESC YTexDesc;
D3DXCONSTANT_DESC UTexDesc;
D3DXCONSTANT_DESC VTexDesc;


//YUV file
FILE *infile = NULL;
unsigned char buf[Width*Height*3/2];
unsigned char *plane[3];
// 
// Structs
//

struct MultiTexVertex
{
	MultiTexVertex(float x, float y, float z,
		float u0, float v0,
		float u1, float v1,
		float u2, float v2)
	{
		 _x =  x;  _y =  y; _z = z;
		_u0 = u0; _v0 = v0; 
		_u1 = u1; _v1 = v1;
		_u2 = u2, _v2 = v2;
	}

	float _x, _y, _z;
	float _u0, _v0;
	float _u1, _v1;
	float _u2, _v2;

	static const DWORD FVF;
};
const DWORD MultiTexVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX3; 

//
// Framework functions
//
bool Setup()
{
	HRESULT hr = 0;

	//
	// Create geometry.
	//

	Device->CreateVertexBuffer(
		6 * sizeof(MultiTexVertex), 
		D3DUSAGE_WRITEONLY,
		MultiTexVertex::FVF,
		D3DPOOL_MANAGED,
		&QuadVB,
		0);

	MultiTexVertex* v = 0;
	QuadVB->Lock(0, 0, (void**)&v, 0);

	v[0] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[1] = MultiTexVertex(-10.0f,  10.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = MultiTexVertex( 10.0f,  10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = MultiTexVertex(-10.0f, -10.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[4] = MultiTexVertex( 10.0f,  10.0f, 5.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = MultiTexVertex( 10.0f, -10.0f, 5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	QuadVB->Unlock();

	//
	// Compile shader
	//

	ID3DXBuffer* shader      = 0;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCompileShaderFromFile(
		"ps_multitex.txt",
		0,
		0,
		"Main", // entry point function name
		"ps_2_0",
		D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		&shader,
		&errorBuffer,
		&MultiTexCT);

	// output any error messages
	if( errorBuffer )
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if(FAILED(hr))
	{
		::MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}

	//
	// Create Pixel Shader
	//
	hr = Device->CreatePixelShader(
		(DWORD*)shader->GetBufferPointer(),
		&MultiTexPS);

	if(FAILED(hr))
	{
		::MessageBox(0, "CreateVertexShader - FAILED", 0, 0);
		return false;
	}

	d3d::Release<ID3DXBuffer*>(shader);

	//
	// Create textures.
	//

	Device->CreateTexture ( Width, Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &YTex, NULL ) ;
	Device->CreateTexture ( Width / 2, Height / 2, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &UTex, NULL ) ;
	Device->CreateTexture ( Width / 2, Height / 2, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &VTex, NULL ) ;

	if((infile=fopen("test_yuv420p_320x180.yuv", "rb"))==NULL){
		printf("cannot open this file\n");
		return false;
	}

	// Set Projection Matrix
	D3DXMATRIX P;
	D3DXMatrixPerspectiveFovLH(
			&P,	D3DX_PI * 0.25f, 
			(float)Width / (float)Height, 1.0f, 1000.0f);

	Device->SetTransform(D3DTS_PROJECTION, &P);

	// Disable lighting.
	Device->SetRenderState(D3DRS_LIGHTING, false); 
	// Get Handles
	YTexHandle      = MultiTexCT->GetConstantByName(0, "YTex");
	UTexHandle		= MultiTexCT->GetConstantByName(0, "UTex");
	VTexHandle		= MultiTexCT->GetConstantByName(0, "VTex");
	// Set constant descriptions:
	UINT count;
	MultiTexCT->GetConstantDesc(YTexHandle,      &YTexDesc, &count);
	MultiTexCT->GetConstantDesc(UTexHandle, &UTexDesc, &count);
	MultiTexCT->GetConstantDesc(VTexHandle,    &VTexDesc, &count);

	MultiTexCT->SetDefaults(Device);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(QuadVB);

	d3d::Release<IDirect3DTexture9*>(YTex);
	d3d::Release<IDirect3DTexture9*>(UTex);
	d3d::Release<IDirect3DTexture9*>(VTex);

	d3d::Release<IDirect3DPixelShader9*>(MultiTexPS);
	d3d::Release<ID3DXConstantTable*>(MultiTexCT);
}

bool Display(float timeDelta)
{
	if (fread(buf, 1, Width*Height*3/2, infile) != Width*Height*3/2){
		// Loop
		fseek(infile, 0, SEEK_SET);
		fread(buf, 1, Width*Height*3/2, infile);
	}

	if( buf != NULL && Device )
	{
		// 
		// Update the scene: Allow user to rotate around scene.
		//
		
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float radius = 20.0f;
		
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			radius -= 2.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			radius += 2.0f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * radius, 0.0f, sinf(angle) * radius );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		Device->SetTransform(D3DTS_VIEW, &V);
		
		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

		plane[0] = buf;
		plane[1] = plane[0] + Width*Height;
		plane[2] = plane[1] + Width*Height/4;

		D3DLOCKED_RECT d3d_rect;
		byte *pSrc = buf;
		//Locks a rectangle on a texture resource.
		//And then we can manipulate pixel data in it.
		LRESULT lRet = YTex->LockRect(0, &d3d_rect, 0, 0);
		if (FAILED(lRet)){
			return false;
		}
		// Copy pixel data to texture
		byte *pDest = (byte *)d3d_rect.pBits;
		int stride = d3d_rect.Pitch; 
		for(int i = 0;i < Height;i ++){
			memcpy(pDest + i * stride,plane[0] + i * Width, Width);
		}

		YTex->UnlockRect(0);

		D3DLOCKED_RECT d3d_rect1;
		lRet = UTex->LockRect(0, &d3d_rect1, 0, 0);
		if (FAILED(lRet)){
			return false;
		}
		// Copy pixel data to texture
		byte *pDest1 = (byte *)d3d_rect1.pBits;
		int stride1 = d3d_rect1.Pitch; 
		for(int i = 0;i < Height/2;i ++){
			memcpy(pDest1 + i * stride1,plane[1] + i * Width / 2, Width / 2);
		}

		UTex->UnlockRect(0);

		D3DLOCKED_RECT d3d_rect2;
		lRet =  VTex->LockRect(0, &d3d_rect2, 0, 0);
		if (FAILED(lRet)){
			return false;
		}
		// Copy pixel data to texture
		byte *pDest2 = (byte *)d3d_rect2.pBits;
		int stride2 = d3d_rect2.Pitch; 
		for(int i = 0;i < Height/2;i ++){
			memcpy(pDest2 + i * stride2,plane[2] + i * Width / 2, Width / 2);
		}

		VTex->UnlockRect(0);


		Device->BeginScene();

		Device->SetPixelShader(MultiTexPS);
		Device->SetFVF(MultiTexVertex::FVF);
		Device->SetStreamSource(0, QuadVB, 0, sizeof(MultiTexVertex));

		// Y tex
		Device->SetTexture(     YTexDesc.RegisterIndex, YTex);
		Device->SetSamplerState(YTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(YTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(YTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(YTexDesc.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		Device->SetSamplerState(YTexDesc.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

		// U tex
		Device->SetTexture(     UTexDesc.RegisterIndex, UTex);
		Device->SetSamplerState(UTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(UTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(UTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(UTexDesc.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		Device->SetSamplerState(UTexDesc.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

		// string tex
		Device->SetTexture(     VTexDesc.RegisterIndex, VTex);
		Device->SetSamplerState(VTexDesc.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(VTexDesc.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(VTexDesc.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(VTexDesc.RegisterIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		Device->SetSamplerState(VTexDesc.RegisterIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

// //
// // WndProc
// //
// LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
// {
// 	switch( msg )
// 	{
// 	case WM_DESTROY:
// 		::PostQuitMessage(0);
// 		break;
		
// 	case WM_KEYDOWN:
// 		if( wParam == VK_ESCAPE )
// 			::DestroyWindow(hwnd);

// 		break;
// 	}
// 	return ::DefWindowProc(hwnd, msg, wParam, lParam);
// }

// //
// // WinMain
// //
// int WINAPI WinMain(HINSTANCE hinstance,
// 				   HINSTANCE prevInstance, 
// 				   PSTR cmdLine,
// 				   int showCmd)
// {
// 	if(!d3d::InitD3D(hinstance,
// 		Width, Height, true, D3DDEVTYPE_HAL, &Device))
// 	{
// 		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
// 		return 0;
// 	}
		
// 	if(!Setup())
// 	{
// 		::MessageBox(0, "Setup() - FAILED", 0, 0);
// 		return 0;
// 	}
// 	d3d::EnterMsgLoop( Display );
// 	Cleanup();
// 	Device->Release();
// 	return 0;
// }