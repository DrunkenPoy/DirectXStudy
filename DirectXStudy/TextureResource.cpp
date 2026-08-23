#include "TextureResource.h"

CTextureResource::CTextureResource() {
  m_targaData = nullptr;
  m_texture = nullptr;
  m_textureView = nullptr;
  m_height = 0;
  m_width = 0;
}

CTextureResource::CTextureResource(const CTextureResource &other) {
  m_targaData = other.m_targaData;
  m_texture = other.m_texture;
  m_textureView = other.m_textureView;
  m_height = other.m_height;
  m_width = other.m_width;
}

CTextureResource::~CTextureResource() {}

bool CTextureResource::Initialize(ID3D11Device *pDevice,
                                  ID3D11DeviceContext *deviceContext,
                                  char *filename) {
  int height, width;
  D3D11_TEXTURE2D_DESC textureDesc;
  UINT rowPitch;
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

  if (!LoadTarga32Bit(filename))
    return false;

  textureDesc.Height = m_height;
  textureDesc.Width = m_width;
  textureDesc.MipLevels = 0;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.SampleDesc.Quality = 0;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

  FAILED_CHECK_RETURN(pDevice->CreateTexture2D(&textureDesc, NULL, &m_texture),
                      false);

  // 타가 이미지의 행피치를 설정
  rowPitch = m_width * 4 * sizeof(UCHAR);

  // 타가 이미지 데이터를 텍스처에 복사
  deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch,
                                   0);

  // 셰이더 리소스 뷰설명 구조체 설정
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = -1;

  FAILED_CHECK_RETURN(
      pDevice->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView),
      false);

  deviceContext->GenerateMips(m_textureView);

  ReleaseArr(m_targaData);

  return true;
}

void CTextureResource::Shutdown() {
  ReleaseCOM_Ptr(m_textureView);
  ReleaseCOM_Ptr(m_texture);
  ReleaseArr(m_targaData);

  return;
}

bool CTextureResource::LoadTarga32Bit(char *filename) {
  int bpp, imageSize, index, k;
  FILE *filePtr;
  TargaHeader targaFileHeader;
  UCHAR *targaImage;

  // 타가 파일을 바이너리 읽기모드로 연다. 파일 스트림 오픈
  if (fopen_s(&filePtr, filename, "rb"))
    return false;

  // 파일 헤더 읽기
  if (fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr) != 1)
    return false;

  // 헤더에서 중요정보를 얻는다.
  m_height = targaFileHeader.height;
  m_width = targaFileHeader.width;
  bpp = targaFileHeader.bpp;

  // 24비트 이미지인지 32비트인지 확인한다.
  if (bpp != 32)
    return false;

  // 이미지 데이터 크기 계산
  imageSize = m_width * m_height * 4;

  // 타가 이미지 데이터를 담을 메모리 할당
  targaImage = new UCHAR[imageSize];

  // 타가 이미지 데이터 읽음.
  if (fread(targaImage, 1, imageSize, filePtr) != imageSize)
    return false;

  // 파일 스트림 닫기
  if (fclose(filePtr))
    return false;

  // 타가 대상 데이터를 담을 메모리를 할당.
  m_targaData = new UCHAR[imageSize];

  index = 0;
  // 타가 이미지 데이터의 인덱스를 초기화
  k = imageSize - (m_width * 4);

  // 타가 포맷은 상하가 뒤집혀 저장되고 rgba순서가 아니여서 이미지 데이터를
  // 재정렬함. 상하 뒤집힘 + BGRA → RGBA 재배열
  for (int i = 0; i < m_height; ++i) {
    for (int j = 0; j < m_width; ++j) {
      m_targaData[index] = targaImage[k + 2];     // R
      m_targaData[index + 1] = targaImage[k + 1]; // G
      m_targaData[index + 2] = targaImage[k];     // B
      m_targaData[index + 3] = targaImage[k + 3]; // A

      k += 4;
      index += 4;
    }

    // 상하가 뒤집혀 읽히므로 targa 이미지 데이터 인덱스를 이전 행의 열 시작
    // 위치로 되돌린다.
    k -= (m_width * 8);
  }

  // 복사 완료이므로, 타가 이미지 데이터 해제
  ReleaseArr(targaImage);

  return true;
}
