#pragma once
#ifndef __TEXTURERESOURCE_H__
#define __TEXTURERESOURCE_H__

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>
#include "myMacro.h"

class CTextureResource
{
	CONSTRUCTION_FEILD_B(CTextureResource,public);
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture() { return m_textureView; }

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

private:
	bool LoadTarga32Bit(char*);

private:
	UCHAR* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
};

#endif