#pragma once
#define CAMERA_MOVE_TIMER 1
#define ASSIMP_BUILD_BOOST_WORKAROUND
/*--------assimp_includes-----------------*/
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>     
/*------------Win_includes----------------*/
#include <windows.h>
#include <vector>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <Objbase.h>
#include <comdef.h>
using namespace std;
/*--------DX_includes-----------------*/
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <Xnamath.h>
/*------------------------------------*/
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3dx11.lib")
#pragma comment (lib, "assimp.lib")
/*

Вывод результата hr: 

		  _com_error err(hr);
          LPCTSTR errMsg = err.ErrorMessage();
		  MessageBox(hwnd,errMsg,L"oops",MB_OK);

*/