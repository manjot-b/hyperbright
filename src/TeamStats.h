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
		TEAM3
	};

	const std::unordered_map<Teams, glm::vec4> colors = {
		{Teams::TEAM0, glm::vec4(.3f, .3f, 1.f, 0.f)},
		{Teams::TEAM1, glm::vec4(.8f, .8f, .3f, 0.f)}
	};
}