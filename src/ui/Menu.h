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

	enum class Selection {
		START,
		EXIT
	};

	enum class ArenaSelection {
		ARENA1,
		ARENA2,
		ARENA3,
		ARENA4,
		LAST	// Not an actual selction. Used to get the selection count
	};

	MainMenu(State state = State::WELCOME, Selection selection = Selection::START, ArenaSelection _arenaSelection = ArenaSelection::ARENA1);
	
	void render();
	
	State getState() const;
	void setState(State state);
	Selection getSelection() const;
	void setSelection(Selection selection);
	ArenaSelection getArenaSelection() const;
	void setArenaSelection(ArenaSelection selection);

private:
	State _state;
	Selection _selection;
	ArenaSelection _arenaSelection;
	std::unique_ptr<openGLHelper::Quad> quad;
	std::shared_ptr<openGLHelper::Texture> arena1;
	std::shared_ptr<openGLHelper::Texture> arena2;
	std::shared_ptr<openGLHelper::Texture> arena3;
	std::shared_ptr<openGLHelper::Texture> arena4;
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
	std::unique_ptr<openGLHelper::Quad> tips;
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