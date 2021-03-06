#include  "TeamStats.h"

namespace teamStats
{
	const std::unordered_map<Teams, glm::vec4> colors = {
		{Teams::TEAM0, glm::vec4(.3f, .3f, 1.f, 0.f)},
		{Teams::TEAM1, glm::vec4(.8f, .8f, .3f, 0.f)}
	};

	std::unordered_map<Teams, unsigned int> scores = {
		{Teams::TEAM0, 0},
		{Teams::TEAM1, 0},
	};
}