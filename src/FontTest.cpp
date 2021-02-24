#include "FontTest.h"

#include <FTGL/ftgl.h>

#include <iostream>

// Rename and/or remove this file. This is only a test to make sure FTGL
// is linked correctly.

namespace ftglTest {
	void test() {
		FTGLPixmapFont font("c:/windows/fonts/arial.ttf");

		if (font.Error())
		{
			std::cerr << "Error: Could not load font.\n";
			return;
		}
		else
		{
			std::cout << "Font successfully loaded.\n";
		}
		// Set the font size and render a small text.
		font.FaceSize(72);
		font.Render("Hello World!");
	}
}