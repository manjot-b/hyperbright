#pragma once

#include <glm/glm.hpp>

#include <unordered_map>

namespace teamStats
{
	enum class Teams
	{
		TEAM0,
		TEAM1,
		TEAM2,
		TEAM3,
	};

	extern const std::unordered_map<Teams, glm::vec4> colors;

	extern std::unordered_map<Teams, unsigned int> scores;
}