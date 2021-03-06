#pragma once
#include <chrono>

namespace ASSB
{
	struct GameTime
	{
	public:
		float MaxDT;
		float DT;
		float RunTime;
		GameTime();

		void Tick();
	private:
		std::chrono::time_point<std::chrono::steady_clock> Start;
		std::chrono::time_point<std::chrono::steady_clock> Last;
	};
}
