# FlameUI [C++ UI Library]

## Aim is to make FlameUI Library super efficient and fast.

<hr>

### <u>For single config generators like MakeFiles, Ninja:</u>

Use cmake while specifying the value of `CMAKE_BUILD_TYPE` cmake variable to be either `Debug` or `Release`

Example: `cmake -DCMAKE_BUILD_TYPE=Debug <path/to/source>`
If not provided with `CMAKE_BUILD_TYPE`, default build configuration selected will be `Debug`

<hr>

### Developer Note: Yet to test build process on Windows. Library currently under development.

<hr>

### Following are all the dependencies for this project, also found in 'FlameUI/vendor':

1. GLFW
2. Glad
3. glm
4. Freetype 2
5. msdfgen

<hr>

Portions of this software are copyright © 2021 The FreeType
Project (www.freetype.org). All rights reserved.

<hr>
