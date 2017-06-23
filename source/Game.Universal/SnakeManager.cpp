#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager) :
		GameComponent(nullptr),
		mSpriteManager(spriteManager)
	{
		XMFLOAT2 dimension = { 6.0f, 3.0f };
		XMINT2 facing = { 1, 0 };
		auto snake = make_shared<Snake>(5, dimension, facing, mSpriteManager);
		mSnakes.push_back(snake);
	}

	void SnakeManager::Update(const DX::StepTimer&)
	{

	}
}
