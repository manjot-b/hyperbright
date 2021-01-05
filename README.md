# Compiling
## Linux
1. First you will need to add all external dependancies as git submodules. To do this enter `git submodule update --init --recursive`
2. It is recommended to create a *build/* directory in the root directory of this project to store the build artifacts. Enter into the *build/* directory and enter `cmake ..` to generate the CMake configuration files.
3. This will generate a Makefile which can be used to compile the project by entering `make -jN`.
