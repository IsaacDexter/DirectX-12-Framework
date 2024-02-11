#pragma once

// win32 & COM
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>

// d3d12
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// helper structures
#include <d3dx12.h>

// dxgi factory
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

// used to load precompiled shaders
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// debug
// tell hlsl compiler to include debug information into shader blob
#if defined (_DEBUG)
//#define D3DCOMPILE_DEBUG 1
#endif // _DEBUG

#include "Helpers.h"

// dx math
#include <DirectXMath.h>