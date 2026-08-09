#include "D3DClass.h"
#include "MacroFunctor.h"

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



bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenFar, float screenNear)
{
    HRESULT result = 0;// 이거 굳이 있을 필요가...?
    IDXGIFactory* factory = nullptr;
    IDXGIAdapter* adapter = nullptr;
    IDXGIOutput* adapterOutput = nullptr;
    uint displayModeLength = 0,numerator = 0, denominator = 0;
    ULONGLONG stringLength = 0;
    DXGI_MODE_DESC* displayModeList = nullptr;
    DXGI_ADAPTER_DESC adapterDesc;
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLvl;
    ID3D11Texture2D* backBufferPtr = nullptr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStecilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    float FoV = 0.f, screenAspect = 0.f;

    //vsync설정을 함수 인풋으로부터 저장
    m_vsync_enabled = vsync;

    MacroFunctor::FHResultCheckbool FaildCheck;


    // //DirectX그래픽 인터페이스 팩토리를 생성한다.
    if (!FaildCheck(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) return false;
    //FAILED_CHECK_RETURN(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory), false); //__uuidof는 처음보는데 찾아봐야겠다.

    // 팩토리로 기본 그래픽 인터페이스(비디오 카드)의 어댑터를 생성한다.
    FAILED_CHECK_RETURN(factory->EnumAdapters(0,&adapter),false);
    
    //기본 어댑터 출력(모니터)을 열거한다.
    FAILED_CHECK_RETURN(adapter->EnumOutputs(0,&adapterOutput), false);

    // 어댑터 출력(모니터)에서 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 포맷에 맞는 모드 개수를 얻는다
    FAILED_CHECK_RETURN(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_ENUM_MODES_INTERLACED,&displayModeLength, NULL), false); // DXGI_ENUM_MODES_INTERLACED는 무슨 디파인인지 아직 감이 안온다.

    // 이 모니터/비디오 카드 조합에서 가능한 모든 디스플레이 모드를 담을 목록을 생성한다.
    displayModeList = new DXGI_MODE_DESC[displayModeLength];
    NULL_CHECK_RETURN(displayModeList, false);
    
    // 이제 디스플레이 모드 목록 구조체를 채운다.
    FAILED_CHECK_RETURN(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &displayModeLength, displayModeList), false);

    // 이제 모든 디스플레이 모드를 훑어 화면 너비·높이와 일치하는 것을 찾는다.
    // 일치하는 것을 찾으면 그 모니터 리프레시율의 분자와 분모를 저장한다.

    for (uint i = 0; i < displayModeLength; ++i)
    {
        if (displayModeList[i].Width == (uint)screenWidth 
            && displayModeList[i].Height == (uint)screenHeight) //C6385 워닝이 뜨는데 인덱스가 안넘어갈 것 같은데도 계속 경고로 나온다 이유를 추후에 찾아봐야할 듯.
        {
            numerator = displayModeList[i].RefreshRate.Numerator;
            denominator = displayModeList[i].RefreshRate.Denominator;
        }
    }

	//어댑터(비디오 카드)의 설명을 얻는다.
	FAILED_CHECK_RETURN(adapter->GetDesc(&adapterDesc), false);

    //전용 비디오 카드 메모리를 메가바이트 단위로 저장한다.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory >> 20); // 1MB = 1024KB = 1024*1024B = 2^20B

    //비디오 카드 이름을 문자 배열로 변환해 저장한다.
    if (wcstombs_s(&stringLength, m_videoCardDesc, 128, adapterDesc.Description, 128))
        return false;

    //디스플레이 모드 목록을 해제한다.
	ReleaseArr(displayModeList);

	adapterOutput->Release();
    adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

    //스왑체인 설정 및 디바이스 생성

	//스왑체인 구조체를 초기화한다.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    //백퍼퍼를 하나로 설정
    swapChainDesc.BufferCount = 1;

	//백버퍼의 너비와 높이를 설정
    swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

    //백퍼퍼를 일반 32비트 서피스로 설정함.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//백버퍼의 리프레시율을 설정한다.
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    //백버퍼의 용도를 설정한다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //렌더링할 창의 핸들을 설정한다.
    swapChainDesc.OutputWindow = hwnd;

    //멀티샘플링을 끈다.
    swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

    //전체화면 또는 창모드로 설정한다.
    if (fullscreen)
        swapChainDesc.Windowed = false;
    else
        swapChainDesc.Windowed = true;

    //스캔라인 순서와 스케일링을 미지정으로 설정한다.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // 표시(present) 후 백버퍼 내용을 폐기한다.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //고급 플래그는 설정하지 않는다.
	swapChainDesc.Flags = 0;

    //기능 수준을 다렉11로 설정한다.
    featureLvl = D3D_FEATURE_LEVEL_11_0;

    // 스왑체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트를 생성한다.
    FAILED_CHECK_RETURN(
        D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        &featureLvl, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain,
        &m_device, NULL, &m_deviceContext),false);

    //백버퍼의 포인터를 얻는다.
    FAILED_CHECK_RETURN(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr), false);

	//백버퍼를 렌더링 타겟 뷰로 생성한다.
    FAILED_CHECK_RETURN(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView), false);

    backBufferPtr->Release();
	backBufferPtr = nullptr;

    //깊이 버퍼의 구조체를 초기화한다.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //깊이 버퍼의 구조체를 설정한다.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    //깊이 버처 구조체로 깊이 버퍼용 텍스처를 생성한다.
    FAILED_CHECK_RETURN(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer), false);

    //스텐실구조체를 초기화한다.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    //깊이 스텐실 구조체를 설정한다.
	depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    //픽셀이 앞면일 때의 스텐실 연산
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//픽셀이 뒷면일 때의 스텐실 연산
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//깊이 스텐실 상태를 생성한다.
    FAILED_CHECK_RETURN(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState), false);

    //깊이 스텐실 상태를 설정한다.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//깊이 스텐실 뷰 구조체를 초기화한다.
    ZeroMemory(&depthStecilViewDesc, sizeof(depthStecilViewDesc));

	//깊이 스텐실 뷰 구조체를 설정한다.
    depthStecilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStecilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStecilViewDesc.Texture2D.MipSlice = 0;

    //깊이 스텐실 뷰를 생성한다.
    FAILED_CHECK_RETURN(m_device->CreateDepthStencilView(m_depthStencilBuffer,&depthStecilViewDesc,&m_depthStencilView), false);

    //렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩한다.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	//래스터라이저 구조체를 초기화한다.
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

    //래스터라이저 구조체를 설정한다.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.f;
    rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.f;

	//래스터라이저 상태를 생성한다.
    FAILED_CHECK_RETURN(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState), false);

	//래스터라이저 상태를 설정한다.
    m_deviceContext->RSSetState(m_rasterState);

	//뷰포트를 설정한다.
	m_viewport.Width = (float)screenWidth;
    m_viewport.Height = (float)screenHeight;
    m_viewport.MinDepth = 0.f;
    m_viewport.MaxDepth = 1.f;
    m_viewport.TopLeftX = 0.f;
    m_viewport.TopLeftY = 0.f;

    //뷰포트를 생성
    m_deviceContext->RSSetViewports(1, &m_viewport);
    
    //투영 행렬 설정
    FoV = PI / 4.f;
    screenAspect = (float)screenWidth / (float)screenHeight;

	//투영 행렬을 생성한다.
    m_projectionMatrix = XMMatrixPerspectiveFovLH(FoV, screenAspect, screenNear, screenFar);

	//월드 행렬을 단위행렬로 설정한다.
    m_worldMatrix = XMMatrixIdentity();

	//직교 행렬을 생성한다.
    m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenFar);
    
	return true;
}

void D3DClass::Shutdown()
{
	//종료 전에 창모드로 설정 후 종료한다. 전체화면 모드에서 종료하면 블랙스크린이 뜨는 경우가 있다.
    if (m_swapChain)
        m_swapChain->SetFullscreenState(false, NULL);

    ReleaseCOM_Ptr(m_rasterState);
    ReleaseCOM_Ptr(m_depthStencilView);
    ReleaseCOM_Ptr(m_depthStencilState);
    ReleaseCOM_Ptr(m_depthStencilBuffer);
    ReleaseCOM_Ptr(m_renderTargetView);
    ReleaseCOM_Ptr(m_deviceContext);
    ReleaseCOM_Ptr(m_device);
    ReleaseCOM_Ptr(m_swapChain);
    
    return;
}

void D3DClass::beginScene(float red = 0.f, float green = 0.f, float blue = 0.f, float alpha = 1.f)
{
    float color[4] = { red,green,blue,alpha };

    //백버퍼 클리어
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    //깊이버퍼 클리어
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

    return;
}

void D3DClass::EndScene()
{
    //렌더링이 끝났으브로 백퍼퍼를 화면에 표시함.

    if (m_vsync_enabled)
        m_swapChain->Present(1, 0); //리프레시율에 맞춰 동기화
    else
		m_swapChain->Present(0, 0); //동기화 없이 바로 표시 앞 변수는 동기화 여부, 뒤 변수는 플래그인데 0이면 기본값으로 동작한다.
    return;
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
