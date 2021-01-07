# Compiling
1. First you will need to add all external dependancies as git submodules. To do this enter `git submodule update --init --recursive`.
2. It is recommended to create a **build/** directory in the root directory of this project to store the build artifacts. Enter into the **build/** directory and enter `cmake ..` to generate the build system files.
3. Build the project with the generated build system files. For example, on Linux a Makefile will be created which can be invoked with `make -jN`. On Windows, a Visual Studio Solution file will be created, which can be opened and built with Visual Studio.
