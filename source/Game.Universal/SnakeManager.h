#pragma once

#include "GameComponent.h"
#include "MatrixHelper.h"
#include <memory>
#include <vector>

namespace DirectXGame
{
	class Snake;

	class SnakeManager final : public DX::GameComponent
	{
	public:
		SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager);
		void Update(const DX::StepTimer& timer) override;

	private:
		std::vector<std::shared_ptr<Snake>> mSnakes;
		std::shared_ptr<SpriteManager> mSpriteManager;
	};
}
