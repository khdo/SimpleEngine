# SimpleEngine
Simple 3D Engine Using OpenGL and glsl

A simple 3D Engine implemented in C++ using OpenGL and glsl that demostrates a
method of painterly rendering. This is implemented using textures to simulate
brush strokes using an additional rendering pass.

# Dependencies

- A graphics card that with a driver that supports OpenGL 2.0 with geometry shader support
- [glew](http://glew.sourceforge.net/)
- [freeglut](http://freeglut.sourceforge.net/)
- [devIL](https://github.com/DentonW/DevIL)

# Models
The 3D models that I used in development of this engine were downloaded from [Turbosquid](https://www.turbosquid.com).
The license for these models does not allow for redistribution so they are
not included as part of this project. The roundabout model can be found [here](https://www.turbosquid.com/FullPreview/Index.cfm/ID/576414).

# Controls
- Movement via arrow keys
- Z and X to move up/down
- V to cycle through brushes
- B to cycle through rendering modes
- P to toggle wireframe mode 
- Esc to quit

# License

This project is licensed under the BSD license (2-clause). It includes 3D 
model loading code from [glObjViewer](http://www.dhpoware.com/demos/glObjViewer.html)
which is licensed under an MIT-style license.
