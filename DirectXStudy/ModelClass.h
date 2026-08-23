#pragma once
#ifndef __MODEL_CLASS_H_
#define __MODEL_CLASS_H_


/*
Include
*/

#include <d3d11.h>
#include <directxmath.h>
#include "myMacro.h"
#include "MacroFunctor.h"

#include "TextureResource.h"

using namespace DirectX;


class ModelClass
{
	private:
		typedef struct VertexType
		{
			VertexType() {
				position = XMFLOAT3(0, 0, 0);
				//color = XMFLOAT4(0, 0, 0, 0);
				texcoordUV0 = XMFLOAT2(0, 0);
			}
			XMFLOAT3 position;
			//XMFLOAT4 color;
			XMFLOAT2 texcoordUV0;
		}tVertexType;
public:
	CONSTRUCTION_FEILD(ModelClass);
	bool Initialize(ID3D11Device*);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture() { return m_texture->GetTexture(); }

private:
		bool InitializeBuffer(ID3D11Device*);
		void ShutdownBuffer();
		void RenderBuffers(ID3D11DeviceContext*);

		bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
		void ReleaseTexture();
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	
	CTextureResource* m_texture;
};


#endif	__MODEL_CLASS_H_