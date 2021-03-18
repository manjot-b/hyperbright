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
	void render();
	
private:
	virtual void renderMenu();
};


//subclasses
class mainMenu : public Menu
{
public:

	enum class State {
		OFF,
		ON
	};

	mainMenu(State state = State::ON);
	virtual void renderMenu();
	virtual State getState() const;
	virtual void setState(State state);

private:
	FTGLPixmapFont font;
	int width, height;
	float defaultFontSize;
	State _state;
};



class pauseMenu : public Menu
{
public:

	enum class PauseSelection {
		RESUME,
		QUIT
	};

	enum class State {
		OFF,
		ON
	};

	pauseMenu(State state = State::OFF, PauseSelection selection = PauseSelection::RESUME);
	virtual void renderMenu();

	virtual PauseSelection getPauseSelection() const;
	virtual void setPauseSelection(PauseSelection selection);
	virtual State getState() const;
	virtual void setState(State state);

private:
	FTGLPixmapFont font;
	int width, height;
	float defaultFontSize;
	PauseSelection pauseSelection;
	State _state;
};




class endMenu : public Menu
{
public:

	enum class State {
		OFF,
		ON
	};

	endMenu(State state = State::OFF);
	virtual void renderMenu();
	virtual State getState() const;
	virtual void setState(State state);

private:
	FTGLPixmapFont font;
	int width, height;
	float defaultFontSize;
	State _state;
};


}	// namespace ui
}	// namespace hyperbright