#pragma once

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <SDKDDKVer.h>
#include <windows.h>
#include <wrl.h>
#include <agile.h>
#include <concrt.h>
#include <memory>

// DirectX
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

// Standard
#include <memory>
#include <string>
#include <cstdint>
#include <sstream>

// Local
#include "VectorHelper.h"
#include "Transform2D.h"
#include "VertexDeclarations.h"
#include "DirectXHelper.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "GamePadComponent.h"