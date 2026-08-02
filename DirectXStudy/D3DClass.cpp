#include "D3DClass.h"


D3DClass::D3DClass()
{
    m_swapChain = nullptr;
    m_device = nullptr;
    m_deviceContext = nullptr;
    m_renderTargetView = nullptr;
    m_depthStencilBuffer = nullptr;
    m_depthStencilState = nullptr;
    m_depthStencilView = nullptr;
    m_rasterState = nullptr;
}

D3DClass::D3DClass(const D3DClass& other)
{
    m_swapChain = nullptr;
    m_device = nullptr;
    m_deviceContext = nullptr;
    m_renderTargetView = nullptr;
    m_depthStencilBuffer = nullptr;
    m_depthStencilState = nullptr;
    m_depthStencilView = nullptr;
    m_rasterState = nullptr;
}

D3DClass::~D3DClass()
{

}



bool D3DClass::Initialize(int, int, bool, HWND, bool, float, float)
{
    return false;
}

void D3DClass::Shutdown()
{
}

void D3DClass::beginScene(float, float, float, float)
{
}

void D3DClass::EndScene()
{
}

ID3D11Device* D3DClass::GetDevice()
{
    return nullptr;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return nullptr;
}

void D3DClass::GetPrjMatrix(XMMATRIX&)
{
}

void D3DClass::GetWorldMatrix(XMMATRIX&)
{
}

void D3DClass::GetOrthoMatrix(XMMATRIX&)
{
}

void D3DClass::GetVideoCardInfo(char*, int&)
{
}

void D3DClass::SetBackBufferRenderTarget()
{
}

void D3DClass::ResetViewport()
{
}
