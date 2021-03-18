#include "Menu.h"

#include <algorithm>
#include <array>
#include <tuple>

#include "engine/TeamStats.h"
#include <iostream>
namespace hyperbright {
namespace ui {
	Menu::Menu()
{}

void Menu::render()
{
	renderMenu();
}


void Menu::renderMenu() {}

//subclasses
	mainMenu::mainMenu(State state) :
		font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(120.f), _state(state)
{}

	pauseMenu::pauseMenu(State state, PauseSelection selection) :
		font("rsc/fonts/neon_pixel-7.ttf"), pauseSelection(selection),  defaultFontSize(120.f), _state(state)
{}

	endMenu::endMenu(State state) :
		font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(120.f), _state(state)
{}


//rendering mainmenu
void mainMenu::renderMenu() {
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetWindowSize(window, &width, &height);
	float scale = (width * 0.1f) / defaultFontSize;
	float xCord = ((float)width / 2) - (5 * (50 * scale) / 2);
	float yCord = ((float)height / 2) - ((50 * scale) / 2);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, -0.28f);
	glPixelTransferf(GL_GREEN_BIAS, -0.89f);
	glPixelTransferf(GL_BLUE_BIAS, -0.13f);
	font.FaceSize(width * 0.1f);
	font.Render("START", -1, FTPoint(xCord, yCord, 0));
	glPopAttrib();
}

mainMenu::State mainMenu::getState() const { return _state; }

void mainMenu::setState(State state) { _state = state; }




//rendering pausemenu
void pauseMenu::renderMenu() {
	if (_state == State::ON) {
		GLFWwindow* window = glfwGetCurrentContext();
		glfwGetWindowSize(window, &width, &height);
		float scale = (width * 0.1f) / defaultFontSize;
		float xCord, yCord, newScale;

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPixelTransferf(GL_RED_BIAS, -0.28f);
		glPixelTransferf(GL_GREEN_BIAS, -0.89f);
		glPixelTransferf(GL_BLUE_BIAS, -0.13f);
		switch (pauseSelection)
		{
		case PauseSelection::RESUME:
			font.FaceSize(1.2 * width * 0.1f);
			newScale = (1.2 * width * 0.1f) / (defaultFontSize);
			xCord = ((float)width / 2) - (6 * (50 * newScale) / 2);
			yCord = ((float)height * 4 / 5);
			font.Render("Resume", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(width * 0.1f);
			xCord = ((float)width / 2) - (4 * (50 * scale) / 2);
			yCord = ((float)height * 3 / 5);
			font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
			break;
		case PauseSelection::QUIT:
			font.FaceSize(width * 0.1f);
			xCord = ((float)width / 2) - (6 * (50 * scale) / 2);
			yCord = ((float)height * 4 / 5);
			font.Render("Resume", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(1.2 * width * 0.1f);
			newScale = (1.2 * width * 0.1f) / (defaultFontSize);
			xCord = ((float)width / 2) - (4 * (50 * newScale) / 2);
			yCord = ((float)height * 3 / 5);
			font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
			break;
		}
		glPopAttrib();
	}
}

pauseMenu::PauseSelection pauseMenu::getPauseSelection() const { return pauseSelection; }

void pauseMenu::setPauseSelection(PauseSelection selection) { pauseSelection = selection; }

pauseMenu::State pauseMenu::getState() const { return _state; }

void pauseMenu::setState(State state) { _state = state; }






//rendering endmenu
void endMenu::renderMenu() {
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetWindowSize(window, &width, &height);
	float scale = (width * 0.1f) / defaultFontSize;
	float xCord, yCord;

	using TeamScore = std::tuple<engine::teamStats::Teams, unsigned int>;
	constexpr size_t count = static_cast<size_t>(engine::teamStats::Teams::LAST);
	std::array<TeamScore, count> sortedScores;

	for (unsigned int i = 0; i < count; i++)
	{
		engine::teamStats::Teams team = static_cast<engine::teamStats::Teams>(i);
		sortedScores[i] = std::make_tuple(team, engine::teamStats::scores[team]);
	}
	std::sort(sortedScores.begin(), sortedScores.end(), [](TeamScore a, TeamScore b) {
		return std::get<1>(a) > std::get<1>(b);
		});

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, -0.28f);
	glPixelTransferf(GL_GREEN_BIAS, -0.89f);
	glPixelTransferf(GL_BLUE_BIAS, -0.13f);

	font.FaceSize(width * 0.1f);
	for (unsigned int i = 0; i < count; i++)
	{
		engine::teamStats::Teams team = std::get<0>(sortedScores[i]);
		std::string nameScore = engine::teamStats::names[team] + ": " + std::to_string(std::get<1>(sortedScores[i]));
		xCord = ((float)width / 2) - (nameScore.length() * (50 * scale) / 2);
		yCord = ((float)height * (4 - i) / 5);
		font.Render(nameScore.c_str(), -1, FTPoint(xCord, yCord, 0));
	}
	font.FaceSize(width * 0.1f);
	xCord = ((float)width / 2) - (19 * (50 * scale) / 2);
	yCord = 0;
	font.Render("Press ENTER to exit", -1, FTPoint(xCord, yCord, 0));

	glPopAttrib();
}

endMenu::State endMenu::getState() const { return _state; }

void endMenu::setState(State state) { _state = state; }


}	// namespace ui
}	// namespace hyperbright