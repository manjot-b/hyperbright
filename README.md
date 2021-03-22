# Compiling
1. First you will need to add all external dependancies as git submodules. To do this enter `git submodule update --init --recursive`.
2. It is recommended to create a **build/** directory in the root directory of this project to store the build artifacts. Enter into the **build/** directory and enter `cmake ..` to generate the build system files.
3. Build the project with the generated build system files. For example, on Linux ~~a Makefile will be created which can be invoked with `make -jN`~~ this does not compile due to PhysX. On Windows, a Visual Studio Solution file will be created, which can be opened and built with Visual Studio.

# Controls
 - Enter to start a game.(Currently there's only one option in start-up menu)
 - W, A, S, D to control the vehicle.
 - Pressing LEFT-SHIFT while turning performs handbrake turning.
 - Pressing Space while a pickup is equiped will activate the pickup.
 - Pressing F if vehicle gets flipped over will attempt to flip the vehicle back over.
 - Pressing C toggles between manual camera and driving camera.
 - Arrow keys to control the manual camera.
 - Camera's point of interest (POI) follows the mouse automatically. 
 - Mouse Scroll to zoom in and out
 - CTRL + SPACE toggles the cursor so you may manage the UI
 - Escape to open a menu, use arrow keys and enter to select.