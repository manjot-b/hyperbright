#include  "TeamStats.h"

namespace hyperbright {
namespace engine {
namespace teamStats {
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

std::unordered_map<Teams, std::string> names = {
	{Teams::TEAM0, "Player"},
	{Teams::TEAM1, "CPU1"},
	{Teams::TEAM2, "CPU2"},
	{Teams::TEAM3, "CPU3"}
};

std::array<TeamScore, teamCount> sortedScores()
{
	std::array<TeamScore, teamCount> sorted;
	for (unsigned int i = 0; i < teamCount; i++)
	{
		engine::teamStats::Teams team = static_cast<engine::teamStats::Teams>(i);
		sorted[i] = std::make_tuple(team, engine::teamStats::scores[team]);
	}
	std::sort(sorted.begin(), sorted.end(), [](TeamScore a, TeamScore b) {
		return std::get<1>(a) > std::get<1>(b);
	});

	return sorted;
}
}	// namespace teamStats
}	// namespace engine
}	// namespace hyperbright