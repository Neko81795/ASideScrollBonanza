#pragma once
#include "Component.hpp"
#include <string>

namespace ASSB
{
	class SpriteComponent : public Component
	{
	public:
		std::wstring Path;
		SpriteComponent(const SpriteComponent &rhs);
		SpriteComponent &operator=(const SpriteComponent &rhs);

		SpriteComponent();
	};
}