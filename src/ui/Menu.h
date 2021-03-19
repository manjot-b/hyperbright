#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>

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

	void updateWindowAndFontSize();
};


//subclasses
class MainMenu : public Menu
{
public:

	enum class State {
		OFF,
		ON
	};

	MainMenu(State state = State::ON);
	void render();
	State getState() const;
	void setState(State state);

private:
	State _state;
};



class PauseMenu : public Menu
{
public:

	enum class Selection {
		RESUME,
		QUIT
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