#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager, const std::shared_ptr<KeyboardComponent>& keyboardComponent,
		const std::shared_ptr<GamePadComponent>& gamePadComponent) :
		GameComponent(nullptr),
		mSpriteManager(spriteManager), mKeyboardComponent(keyboardComponent), mGamePadComponent(gamePadComponent)
	{
		XMFLOAT2 dimension = { 6.0f, 3.0f };
		XMINT2 facing = { 1, 0 };
		auto snake = make_shared<Snake>(15, dimension, facing, mSpriteManager);
		snake->SetHeadingDirection(XMFLOAT2(-1.0f, 0.0f));
		mSnakes.push_back(snake);
	}

	void SnakeManager::Update(const DX::StepTimer& timer)
	{
		XMFLOAT2 headingOffset = { 0.0f, 0.0f };
		if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Up))
		{
			headingOffset.y = 1.0f;
		}
		else if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Down))
		{
			headingOffset.y = -1.0f;
		}

		if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Right))
		{
			headingOffset.x = 1.0f;
		}
		else if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Left))
		{
			headingOffset.x = -1.0f;
		}

		for (auto& snake : mSnakes)
		{
			if ((headingOffset.x + headingOffset.y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffset);
			}
			snake->Update(timer);
		}
	}
}
