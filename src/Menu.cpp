#include "Menu.h"

#include <algorithm>
#include <array>
#include <tuple>

#include "TeamStats.h"

Menu::Menu(State state, PauseSelection selection) :
	font("rsc/fonts/ROGFonts-Regular.otf"), _state(state), pauseSelection(selection)
{}

void Menu::render()
{
	switch (_state)
	{
	case State::MAIN:
		renderMain(); break;
	case State::PAUSE:
		renderPause(); break;
	case State::END:
		renderEnd();  break;
	case State::NONE: break;
	}
}

void Menu::renderMain()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, -1);
	glPixelTransferf(GL_BLUE_BIAS, -1);
	font.FaceSize(70);
	font.Render("START", -1, FTPoint(425, 400, 0));//(window size / 2) - ((string length * FontSize) / 4) + (Fontsize / 2)
	glPopAttrib();
}

void Menu::renderPause()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, -1);
	glPixelTransferf(GL_BLUE_BIAS, -1);
	switch (pauseSelection)
	{
	case PauseSelection::RESUME:
		font.FaceSize(70);
		font.Render("Resume", -1, FTPoint(425, 400, 0));//(window size / 2) - ((string length * FontSize) / 4) + (Fontsize / 2)
		font.FaceSize(50);
		font.Render("Quit", -1, FTPoint(525, 300, 0));
		break;
	case PauseSelection::QUIT:
		font.FaceSize(50);
		font.Render("Resume", -1, FTPoint(475, 400, 0));
		font.FaceSize(70);
		font.Render("Quit", -1, FTPoint(495, 300, 0));
		break;
	}
	glPopAttrib();
}

void Menu::renderEnd()
{
	using TeamScore = std::tuple<teamStats::Teams, unsigned int>;
	constexpr size_t count = static_cast<size_t>(teamStats::Teams::LAST);
	std::array<TeamScore, count> sortedScores;

	for (unsigned int i = 0; i < count; i++)
	{
		teamStats::Teams team = static_cast<teamStats::Teams>(i);
		sortedScores[i] = std::make_tuple(team, teamStats::scores[team]);
	}
	std::sort(sortedScores.begin(), sortedScores.end(), [](TeamScore a, TeamScore b) {
		return std::get<1>(a) > std::get<1>(b);
	});

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, 0);
	glPixelTransferf(GL_GREEN_BIAS, -1);
	glPixelTransferf(GL_BLUE_BIAS, -1);

	font.FaceSize(70);
	for (unsigned int i = 0; i < count; i++)
	{
		teamStats::Teams team = std::get<0>(sortedScores[i]);
		std::string nameScore = teamStats::names[team] + ": " + std::to_string(std::get<1>(sortedScores[i]));
		font.Render(nameScore.c_str(), -1, FTPoint(450, 500 - i*100, 0));
	}
	font.FaceSize(50);
	font.Render("Press ENTER to exit", -1, FTPoint(250, 50, 0));

	glPopAttrib();
}

Menu::State Menu::getState() const { return _state; }

void Menu::setState(State state) { _state = state; }

Menu::PauseSelection Menu::getPauseSelection() const { return pauseSelection; }

void Menu::setPauseSelection(PauseSelection selection) { pauseSelection = selection; }