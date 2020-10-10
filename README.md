Ensure the following libraries are globally installed or located in the *lib* at the root of this project.
- opengl
- glfw3
- assimp

Assimp requires a symbolic linking structure like so,
libassimp.so -> libassimp.so.X
libassimp.so.X -> libassimp.so.X.Y.Z
libassimp.X.Y.Z

Where libassimp.X.Y.Z is the actual shared library file. (*TODO* Look into make libassimp a git submodule and have it automatically build and be placed in the correct location.) 
