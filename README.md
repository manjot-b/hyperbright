# Compiling
1. First you will need to add all external dependancies as git submodules. To do this enter `git submodule update --init --recursive`.
2. It is recommended to create a **build/** directory in the root directory of this project to store the build artifacts. Enter into the **build/** directory and enter `cmake ..` to generate the build system files.
3. Build the project with the generated build system files. For example, on Linux ~~a Makefile will be created which can be invoked with `make -jN`~~ this does not compile due to PhysX. On Windows, a Visual Studio Solution file will be created, which can be opened and built with Visual Studio.

# Controls
 - Upon start-up press ENTER to take you to the Arena selection screen where you can use left and right ARROWS to choose and ENTER again to select.
 - W, A, S, D to control the vehicle.
 - S will first perform regular braking until the vehicle slows to almost a stop where it will switch into reverse.
 - Pressing LEFT-SHIFT while turning performs handbrake turning.
 - Pressing Space while a pickup is equiped will activate the pickup.
 - Pressing F if vehicle gets flipped over will attempt to flip the vehicle back over.
 - Pressing C toggles between manual camera and driving camera.
 - Arrow keys to control the manual camera.
 - Camera's point of interest follows the mouse automatically. 
 - Mouse Scroll to zoom in and out
 - CTRL + SPACE toggles the cursor so you may manage the UI
 - Escape to open a menu, use arrow keys and enter to select.

# DevUI window
 - Music volume slider
 - Number of graphics settings
 - Record of team scores
 - Fps display and set maximum.

# Pickups
 - Upon colliding with a pickup in the arena you will equip a random pickup

Pickup#1: EMP
 - All vehicles energy will be set to zero

Pickup#2: ZAP
 - The vehicle with the most tiles colored will temporarily switch the color they lay down to match the color of the vehicle that activated ZAP.

Pickup#3: SPEED
 - Will give you a temporary and sustained speed boost, where turning becomes more difficult and flipping over can easily occur.

Pickup#4: SLOWTRAP
 - The tile below you when activated will become a trap that temporarily slows the next vehicle who drives upon it.  Short set delay so that you don't immediately trigger the trap.

Pickup#5: SYPHON
 - Upon activation you no longer put down color but you remove color from tiles and restore your vehicles energy.

# HUD
 - Round timer top left
 - When the vehicle has a pickup equipped a specific pickup texture will appear below round timer.
 - Live mini-map top right
 - Bottom left display of bars indicates how many tiles you have energy left to trigger.
 - Speedometer on bottom right.