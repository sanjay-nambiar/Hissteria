#pragma once

// Windows
#include <wrl.h>
#include <agile.h>
#include <concrt.h>
#include <ppltasks.h>

// DirectX
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <Initguid.h>
#include <dxgidebug.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>

// Standard
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

// Library
#include "ColorHelper.h"
#include "DirectXHelper.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "DeviceResources.h"
#include "GameComponent.h"
#include "DrawableGameComponent.h"
#include "DirectXHelper.h"
#include "FpsTextRenderer.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "Transform2D.h"
#include "VertexDeclarations.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "GamePadComponent.h"

// Local
#include "Ball.h"
#include "BallManager.h"
#include "Field.h"
#include "FieldManager.h"
#include "MoodySprite.h"
#include "SpriteDemoManager.h"

// Game
#include "Sprite.h"
#include "Spawn.h"
#include "SpawnManager.h"
#include "Snake.h"
#include "SnakeManager.h"
