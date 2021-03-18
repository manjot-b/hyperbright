#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>

namespace hyperbright {
namespace ui {
class Menu
{
public:
	enum class State {
		NONE,
		MAIN,
		PAUSE,
		END
	};

	enum class PauseSelection {
		RESUME,
		QUIT
	};

	Menu(State state = State::MAIN, PauseSelection selection = PauseSelection::RESUME);
	void render();

	State getState() const;
	void setState(State state);
	PauseSelection getPauseSelection() const;
	void setPauseSelection(PauseSelection selection);

private:
	FTGLPixmapFont font;
	State _state;
	PauseSelection pauseSelection;
	int width, height;
	float defaultFontSize;

	void renderMain();
	void renderPause();
	void renderEnd();
};
}	// namespace ui
}	// namespace hyperbright