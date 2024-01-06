# FlameUI [C++ UI Library]

Aim is to make a User Interface Library which will be super efficient and fast. This is made purely using OpenGL and C++, aiming to creating a user interface library ground up.

### **Important: This project is not matured or meant to be actually used by a user to integrate in his/her own project. I made this project when I was quite young so this repository is here to showcase what I did, and not a production ready user interface library.**

***
<video src="README/FlameUI_Showcase.mov" autoplay loop muted></video>

## Features
1. Multiple Panel Support (Panel Selection System)
2. Panel resizing
3. Title Bar for every Panel
4. Text Rendering (In previous commits, PS: This is an unprofessional way to mention a feature)
5. Basic Button Behaviour (Demonstrated in the Video)
6. Attempt to make a Docking System

### For single config generators like MakeFiles, Ninja:

Use cmake while specifying the value of `CMAKE_BUILD_TYPE` cmake variable to be either `Debug` or `Release`

Example: `cmake -DCMAKE_BUILD_TYPE=Debug <path/to/source>`
If not provided with `CMAKE_BUILD_TYPE`, default build configuration selected will be `Debug`

<hr>

_Developer Note: Yet to test build process on Windows. Library currently under development._


### Following are all the dependencies for this project, also found in 'FlameUI/vendor':

1. GLFW
2. Glad
3. glm
4. Freetype 2
5. msdfgen

<hr>

Portions of this software are copyright © 2021 The FreeType
Project (www.freetype.org). All rights reserved.
