#pragma once
#ifndef __APPLICATION_CLASS_H__
#define __APPLICATION_CLASS_H__

#include <Windows.h>
#include "myMacro.h"

#include "D3DClass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "TextureShader.h"

const bool g_FullScreen = false;
const bool g_VsyncEnabled = true;
const float g_ScreenFar = 1000.0f;
const float g_ScreenNear= 1.0f;

#define FULL_SCREEN g_FullScreen
#define VSYNC_ENABLED  g_VsyncEnabled
#define SCREEN_DEPTH  g_ScreenFar
#define SCREEN_NEAR  g_ScreenNear


class ApplicationClass
{
	CONSTRUCTION_FEILD(ApplicationClass)
public:
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
private:
	bool Render();

private:
	D3DClass* m_direct3D; 
	CameraClass* m_camera;
	ModelClass* m_model;
	//ColorShaderClass* m_colorShader;
	CTextureShader* m_textureShader;

};

#endif __APPLICATION_CLASS_H__
