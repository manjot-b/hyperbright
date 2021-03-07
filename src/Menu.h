#pragma once

#include <FTGL/ftgl.h>

#include "Renderer.h"

class Menu : Renderer::IRenderable
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
	void render(const Shader& shader);

	State getState() const;
	void setState(State state);
	PauseSelection getPauseSelection() const;
	void setPauseSelection(PauseSelection selection);

private:
	FTGLPixmapFont font;
	State _state;
	PauseSelection pauseSelection;

	void renderMain();
	void renderPause();
	void renderEnd();
};