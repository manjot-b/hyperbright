#include  "TeamStats.h"

namespace teamStats
{
	const std::unordered_map<Teams, glm::vec4> colors = {
		{Teams::TEAM0, glm::vec4(.3f, .3f, 1.f, 0.f)},
		{Teams::TEAM1, glm::vec4(.9f, .9f, .3f, 0.f)},
		{Teams::TEAM2, glm::vec4(.2f, .96f, .34f, 0.f)},
		{Teams::TEAM3, glm::vec4(.83f, .2f, .92f, 0.f)}
	};

	std::unordered_map<Teams, unsigned int> scores = {
		{Teams::TEAM0, 0},
		{Teams::TEAM1, 0},
		{Teams::TEAM2, 0},
		{Teams::TEAM3, 0}
	};
}