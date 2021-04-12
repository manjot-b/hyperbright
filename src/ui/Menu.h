#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <FTGL/ftgl.h>
#include "opengl-helper/FrameBuffer.h"
#include "opengl-helper/Quad.h"
#include "opengl-helper/Texture.h"

namespace hyperbright {
namespace ui {
class Menu
{
public:
	Menu();
	virtual void render() = 0;
	
protected:
	FTGLPixmapFont font;
	float defaultFontSize;
	unsigned int width, height;
	glm::vec3 color;
	std::shared_ptr<openGLHelper::Shader> quadShader;
	std::unique_ptr<openGLHelper::Quad> tips;

	void updateWindowAndFontSize();
};


//subclasses
class MainMenu : public Menu
{
public:

	enum class State {
		OFF,
		WELCOME,
		SETUP
	};

	enum class ArenaSelection {
		ARENA1,
		ARENA2,
		ARENA3,
		ARENA4,
		LAST	// Not an actual selction. Used to get the selection count
	};

	enum class ControllerSelection {
		XBOX,
		PS4,
		NONE
	};

	enum class Selection {
		CONTROLLER,
		ARENA
	};

	MainMenu(State state = State::WELCOME, ArenaSelection _arenaSelection = ArenaSelection::ARENA1);
	
	void render();
	
	State getState() const;
	void setState(State state);
	ArenaSelection getArenaSelection() const;
	void setArenaSelection(ArenaSelection selection);
	ControllerSelection getControllerSelection() const;
	void setControllerSelection(ControllerSelection controller);
	Selection getSelection() const;
	void setSelection(Selection select);

private:
	State _state;
	ArenaSelection _arenaSelection;
	ControllerSelection _controllerSelection;
	Selection _selection;
};



class PauseMenu : public Menu
{
public:

	enum class Selection {
		RESUME = 0,
		MAIN_MENU,
		QUIT,
		LAST	// Not an actual selction. Used to get the selection count
	};

	enum class State {
		OFF,
		ON
	};

	PauseMenu(State state = State::OFF, Selection selection = Selection::RESUME);
	void render();

	Selection getSelection() const;
	void setSelection(Selection selection);
	State getState() const;
	void setState(State state);

private:
	Selection _selection;
	State _state;
};


class EndMenu : public Menu
{
public:

	enum class State {
		OFF,
		ON
	};

	enum class Selection {
		MAIN_MENU,
		QUIT
	};

	EndMenu(State state = State::OFF, Selection = Selection::MAIN_MENU);
	void render();
	Selection getSelection() const;
	void setSelection(Selection selection);
	State getState() const;
	void setState(State state);

private:
	State _state;
	Selection _selection;
};


}	// namespace ui
}	// namespace hyperbright