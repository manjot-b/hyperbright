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
	int width, height;
	float defaultFontSize;
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

	EndMenu(State state = State::OFF);
	void render();
	State getState() const;
	void setState(State state);

private:
	State _state;
};


}	// namespace ui
}	// namespace hyperbright