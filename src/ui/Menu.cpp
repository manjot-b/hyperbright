#include "Menu.h"

#include <algorithm>
#include <array>
#include <tuple>
#include <iostream>

#include "engine/TeamStats.h"
#include "render/Renderer.h"

namespace hyperbright {
namespace ui {

/*
The base Menu class. All other Menu must derive from this class.
*/
Menu::Menu() : font("rsc/fonts/neon_pixel-7.ttf"), defaultFontSize(100.f), width(0), height(0), color(0.72f, 0.11f, 0.87f)
{
	render::Renderer::getInstance().getWindowSize(width, height);
}

void Menu::updateWindowAndFontSize()
{
	render::Renderer::getInstance().getWindowSize(width, height);
	//defaultFontSize = width * 0.05;
}

MainMenu::MainMenu(State state, ArenaSelection arenaSelection) : Menu(),
	_state(state), _arenaSelection(arenaSelection)
{}

void MainMenu::render() {
	updateWindowAndFontSize();
	float scale = (width * 0.1f) / defaultFontSize;
	float scaleSmall = scale * .7;
	float xCoord, yCoord;
	const unsigned float rows = 10;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, color.r - 1);
	glPixelTransferf(GL_GREEN_BIAS, color.g - 1);
	glPixelTransferf(GL_BLUE_BIAS, color.b - 1);
	
	switch (_state)
	{
	case hyperbright::ui::MainMenu::State::WELCOME:
		xCoord = ((float)width / 2) - (5 * (50 * scale) / 2);
		yCoord = ((float)height / 2) - ((50 * scale) / 2);
		font.FaceSize(scale * defaultFontSize);
		font.Render("START", -1, FTPoint(xCoord, yCoord, 0));
		break;
	case hyperbright::ui::MainMenu::State::SETUP:
		xCoord = ((float)width / 2) - (5 * (50 * scale) / 2);
		yCoord = (float)height *  (5.f / rows);
		font.FaceSize(scale * defaultFontSize);
		font.Render("SETUP", -1, FTPoint(xCoord, yCoord, 0));

		xCoord = ((float)width / 2) - (11 * (50 * scaleSmall) / 2);
		yCoord = (float)height * (2.f / rows);
		font.FaceSize(scaleSmall * defaultFontSize);
		const std::string arena = "< Arena " + std::to_string(static_cast<int>(_arenaSelection)) + " >";
		font.Render(arena.c_str(), -1, FTPoint(xCoord, yCoord, 0));

		xCoord = ((float)width / 2) - (11 * (50 * scaleSmall) / 2);
		yCoord = (float)height * (0.f / rows);
		font.Render("Press enter", -1, FTPoint(xCoord, yCoord, 0));
		break;
	}
	
	glPopAttrib();
}

MainMenu::State MainMenu::getState() const { return _state; }

void MainMenu::setState(State state) { _state = state; }

MainMenu::ArenaSelection MainMenu::getArenaSelection() const { return _arenaSelection; }

void MainMenu::setArenaSelection(ArenaSelection selection) { _arenaSelection = selection; }


PauseMenu::PauseMenu(State state, Selection selection) : _selection(selection), _state(state) {}

void PauseMenu::render() {
	if (_state == State::ON) {
		updateWindowAndFontSize();
		float scale = (width * 0.1f) / defaultFontSize;
		float scaleBig = scale * 1.2f;
		float xCord, yCord;

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPixelTransferf(GL_RED_BIAS, color.r - 1);
		glPixelTransferf(GL_GREEN_BIAS, color.g - 1);
		glPixelTransferf(GL_BLUE_BIAS, color.b - 1);
		switch (_selection)
		{
		case Selection::RESUME:
			font.FaceSize(scaleBig * defaultFontSize);
			xCord = ((float)width / 2) - (6 * (50 * scaleBig) / 2);
			yCord = ((float)height * 4 / 5);
			font.Render("Resume", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (9 * (50 * scale) / 2);
			yCord = ((float)height * 3 / 5);
			font.Render("Main Menu", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (4 * (50 * scale) / 2);
			yCord = ((float)height * 2 / 5);
			font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
			break;
		case Selection::MAIN_MENU:
			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (6 * (50 * scale) / 2);
			yCord = ((float)height * 4 / 5);
			font.Render("Resume", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scaleBig * defaultFontSize);
			xCord = ((float)width / 2) - (9 * (50 * scaleBig) / 2);
			yCord = ((float)height * 3 / 5);
			font.Render("Main Menu", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (4 * (50 * scale) / 2);
			yCord = ((float)height * 2 / 5);
			font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
			break;
		case Selection::QUIT:
			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (6 * (50 * scale) / 2);
			yCord = ((float)height * 4 / 5);
			font.Render("Resume", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scale * defaultFontSize);
			xCord = ((float)width / 2) - (9 * (50 * scale) / 2);
			yCord = ((float)height * 3 / 5);
			font.Render("Main Menu", -1, FTPoint(xCord, yCord, 0));

			font.FaceSize(scaleBig * defaultFontSize);
			xCord = ((float)width / 2) - (4 * (50 * scaleBig) / 2);
			yCord = ((float)height * 2 / 5);
			font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
			break;
		}
		glPopAttrib();
	}
}

PauseMenu::Selection PauseMenu::getSelection() const { return _selection; }

void PauseMenu::setSelection(Selection selection) { _selection = selection; }

PauseMenu::State PauseMenu::getState() const { return _state; }

void PauseMenu::setState(State state) { _state = state; }


EndMenu::EndMenu(State state, Selection selection) : _state(state), _selection(selection) {}

void EndMenu::render() {
	updateWindowAndFontSize();
	float scale = (width * 0.07f) / defaultFontSize;
	float scaleBig = scale * 1.1f;
	float xCord, yCord;
	const unsigned int rows = 10;

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

	glPixelTransferf(GL_RED_BIAS, color.r - 1);
	glPixelTransferf(GL_GREEN_BIAS, color.g - 1);
	glPixelTransferf(GL_BLUE_BIAS, color.b - 1);

	font.FaceSize(scale * defaultFontSize);
	for (unsigned int i = 0; i < count; i++)
	{
		engine::teamStats::Teams team = std::get<0>(sortedScores[i]);
		std::string nameScore = engine::teamStats::names[team] + ": " + std::to_string(std::get<1>(sortedScores[i]));
		xCord = ((float)width / 2) - (nameScore.length() * (50 * scale) / 2);
		yCord = ((float)height * ((rows - 2) - i) / rows);	// 1 row of padding at the top
		font.Render(nameScore.c_str(), -1, FTPoint(xCord, yCord, 0));
	}

	switch (_selection)
	{
	case Selection::MAIN_MENU:
		font.FaceSize(scaleBig * defaultFontSize);
		xCord = ((float)width / 2) - (9 * (50 * scaleBig) / 2);
		yCord = ((float)height * 2 / rows);
		font.Render("Main Menu", -1, FTPoint(xCord, yCord, 0));

		font.FaceSize(scale * defaultFontSize);
		xCord = ((float)width / 2) - (4 * (50 * scale) / 2);
		yCord = ((float)height * 1 / rows);
		font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
		break;
	case Selection::QUIT:
		font.FaceSize(scale * defaultFontSize);
		xCord = ((float)width / 2) - (9 * (50 * scale) / 2);
		yCord = ((float)height * 2 / rows);
		font.Render("Main Menu", -1, FTPoint(xCord, yCord, 0));

		font.FaceSize(scaleBig * defaultFontSize);
		xCord = ((float)width / 2) - (4 * (50 * scaleBig) / 2);
		yCord = ((float)height * 1 / rows);
		font.Render("Quit", -1, FTPoint(xCord, yCord, 0));
		break;
	}

	glPopAttrib();
}

EndMenu::Selection EndMenu::getSelection() const { return _selection; }

void EndMenu::setSelection(Selection selection) { _selection = selection; }

EndMenu::State EndMenu::getState() const { return _state; }

void EndMenu::setState(State state) { _state = state; }

}	// namespace ui
}	// namespace hyperbright