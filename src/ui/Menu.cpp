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
Menu::Menu() :
	font("rsc/fonts/neon_pixel-7.ttf"),
	defaultFontSize(100.f), width(0), height(0),
	color(0.72f, 0.11f, 0.87f),
	quadShader(std::make_shared<openGLHelper::Shader>("rsc/shaders/quad_vertex.glsl", "rsc/shaders/quad_fragment.glsl"))
{
	render::Renderer::getInstance().getWindowSize(width, height);
	quadShader->link();
}

void Menu::updateWindowAndFontSize()
{
	render::Renderer::getInstance().getWindowSize(width, height);
	//defaultFontSize = width * 0.05;
}

MainMenu::MainMenu(const std::vector<std::shared_ptr<entity::Arena>>& _arenas,
		State state, Selection selection,
		ArenaSelection arenaSelection) : Menu(),
	arenas(_arenas), _state(state), _selection(selection), _arenaSelection(arenaSelection),
	arena1(std::make_shared<openGLHelper::Texture>("rsc/images/arena1_map.png")),
	arena2(std::make_shared<openGLHelper::Texture>("rsc/images/arena2_map.png")),
	arena3(std::make_shared<openGLHelper::Texture>("rsc/images/arena3_map.png")),
	arena4(std::make_shared<openGLHelper::Texture>("rsc/images/arena4_map.png")),
	difficultyColor(0.8f, 0.5f, 0.3f)
	
{
	// Don't why this can't be constrcuted in the initialzer list.
	quad = std::make_unique<openGLHelper::Quad>(quadShader, arena1);
	glm::vec2 scale = glm::vec2(1.f, (float)width / height);
	scale *= .5f;
	quad->scale(scale);
	quad->translate(glm::vec2(.9f - quad->getWidth() * .5f, -.25f) / scale);
	difficulties = { "BEGINNER", "NORMAL", "HARDCORE" };
}

void MainMenu::render() {
	updateWindowAndFontSize();
	float scale = (width * 0.055f) / defaultFontSize;
	float scaleBig = scale * 1.2f;
	float scaleSmall = scale * .7;
	float xCoord, yCoord;
	const float rows = 10;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPixelTransferf(GL_RED_BIAS, color.r - 1);
	glPixelTransferf(GL_GREEN_BIAS, color.g - 1);
	glPixelTransferf(GL_BLUE_BIAS, color.b - 1);

	xCoord = ((float)width / 2) - (12 * (50 * scaleBig) / 2);
	yCoord = ((float)height * 3.8 / 5);
	font.FaceSize(1.5f * scaleBig * defaultFontSize);
	font.Render("HYPER", -1, FTPoint(xCoord, yCoord, 0));

	xCoord = ((float)width / 2) - (10 * (50 * scaleBig) / 2);
	yCoord = ((float)height * 3.1 / 5);
	font.FaceSize(1.5f * scaleBig * defaultFontSize);
	font.Render("~~~ BRIGHT", -1, FTPoint(xCoord, yCoord, 0));
	
	switch (_state)
	{
	case State::WELCOME:
	{
		switch (_selection)
		{
		case Selection::START:
			xCoord = ((float)width / 2) - (4 * (50 * scaleBig) / 2);
			yCoord = ((float)height * 2 / 5) + ((50 * scaleBig));
			font.FaceSize(scaleBig * defaultFontSize);
			font.Render("START", -1, FTPoint(xCoord, yCoord, 0));

			xCoord = ((float)width / 2) - (3 * (50 * scale) / 2);
			yCoord = ((float)height * 1.5 / 5) + ((50 * scale));
			font.FaceSize(scale * defaultFontSize);
			font.Render("QUIT", -1, FTPoint(xCoord, yCoord, 0));
			break;
		case Selection::EXIT:
			xCoord = ((float)width / 2) - (4 * (50 * scale) / 2);
			yCoord = ((float)height * 2 / 5) + ((50 * scale));
			font.FaceSize(scale * defaultFontSize);
			font.Render("START", -1, FTPoint(xCoord, yCoord, 0));

			xCoord = ((float)width / 2) - (3 * (50 * scaleBig) / 2);
			yCoord = ((float)height * 1.5 / 5) + ((50 * scaleBig));
			font.FaceSize(scaleBig * defaultFontSize);
			font.Render("QUIT", -1, FTPoint(xCoord, yCoord, 0));
			break;
		}
		glPopAttrib();
	}
	break;
	case State::SETUP:
		xCoord = ((float)width / 2) - (5 * (50 * scale) / 2);
		yCoord = (float)height *  (3.f / rows);
		font.FaceSize(scaleBig * defaultFontSize);
		font.Render("ARENA", -1, FTPoint(xCoord, yCoord, 0));

		xCoord = ((float)width / 2) - (14 * (50 * scaleSmall) / 2);
		yCoord = (float)height * (1.5f / rows);
		font.FaceSize(scale * defaultFontSize);
		const std::string arena = "< Arena " + std::to_string(static_cast<int>(_arenaSelection) + 1) + " >";
		font.Render(arena.c_str(), -1, FTPoint(xCoord, yCoord, 0));

		xCoord = ((float)width / 2) - (10 * (50 * scaleSmall) / 2);
		yCoord = (float)height * (0.f / rows);
		font.FaceSize(scaleSmall * defaultFontSize);
		font.Render("Press enter", -1, FTPoint(xCoord, yCoord, 0));

		glPixelTransferf(GL_RED_BIAS, difficultyColor.r - 1);
		glPixelTransferf(GL_GREEN_BIAS, difficultyColor.g - 1);
		glPixelTransferf(GL_BLUE_BIAS, difficultyColor.b - 1);
		entity::Arena::Difficulty arenaDifficulty = arenas[static_cast<int>(_arenaSelection)]->getDifficulty();
		const std::string& diff = difficulties[static_cast<int>(arenaDifficulty)];
		xCoord = width - (diff.length() + 10) * (50 * scaleSmall) / 2;
		yCoord = (float)height * (1.f / rows);
		font.FaceSize(scaleSmall * defaultFontSize);
		font.Render(diff.c_str(), -1, FTPoint(xCoord, yCoord, 0));

		glPopAttrib();

		quad->getShader()->use();
		quad->render();
		glUseProgram(0);
		break;
	}
}

MainMenu::State MainMenu::getState() const { return _state; }

void MainMenu::setState(State state) { _state = state; }

MainMenu::Selection MainMenu::getSelection() const { return _selection; }

void MainMenu::setSelection(Selection selection) { _selection = selection; }

MainMenu::ArenaSelection MainMenu::getArenaSelection() const { return _arenaSelection; }

void MainMenu::setArenaSelection(ArenaSelection selection)
{
	_arenaSelection = selection;
	switch (_arenaSelection)
	{
	case ArenaSelection::ARENA1:
		quad->setTexture(arena1);
		break;
	case ArenaSelection::ARENA2:
		quad->setTexture(arena2);
		break;
	case ArenaSelection::ARENA3:
		quad->setTexture(arena3);
		break;
	case ArenaSelection::ARENA4:
		quad->setTexture(arena4);
		break;
	}
}


PauseMenu::PauseMenu(State state, Selection selection) :
	_selection(selection), _state(state),
	tips(std::make_unique<openGLHelper::Quad>(quadShader, std::make_shared<openGLHelper::Texture>("rsc/images/tips.png")))
{}

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

		tips->getShader()->use();
		tips->normalizeToViewport(width, height);
		tips->translate(glm::vec2(0.f, -0.8f));
		tips->scale(1.2f);
		tips->render();
		glUseProgram(0);
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