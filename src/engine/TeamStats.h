#pragma once

#include <glm/glm.hpp>

#include <array>
#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_map>

namespace hyperbright {
namespace engine {
namespace teamStats {
enum class Teams
{
	TEAM0,	// player team
	TEAM1,
	TEAM2,
	TEAM3,
	LAST	// Not an actual team. Used to iterator over enum.
};

constexpr unsigned int teamCount = 4;

extern const std::unordered_map<Teams, glm::vec4> colors;

extern std::unordered_map<Teams, unsigned int> scores;

extern std::unordered_map<Teams, std::string> names;

using TeamScore = std::tuple<engine::teamStats::Teams, unsigned int>;
std::array<TeamScore, teamCount> sortedScores();
}	// namespace teamStats
}	// namespace engine
}	// namespace hyperbright