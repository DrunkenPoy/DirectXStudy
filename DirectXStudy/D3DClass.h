#pragma once
#ifndef __D3DCLASS_H__
#define __D3DCLASS_H__

/*
lib linking
*/

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

/*
Include 
*/

#include <d3d11.h>
#include <directxmath.h>
#include "myMacro.h"

using namespace DirectX;




class D3DClass
{
	CONSTRUCTION_FEILD(D3DClass)
public:
	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void beginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetPrjMatrix(XMMATRIX&); //XMMATRIX는 4x4 매트릭스이다.
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void SetBackBufferRenderTarget();
	void ResetViewport();
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDesc[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	D3D11_VIEWPORT m_viewport;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

};


#endif