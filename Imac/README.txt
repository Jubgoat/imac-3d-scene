# Textured iMac 3D Scene

C++ and OpenGL desk scene built for CS 330 (SNHU). Procedural textures, two Phong lights, free camera, perspective and orthographic views.

## Build and run (Windows, Visual Studio 2022)

1. Install Visual Studio 2022 with the "Desktop development with C++" workload (includes CMake).
2. File > Open > Folder, and choose this folder.
3. Wait for CMake to finish configuring (it reports a clear error if a library file is missing).
4. Select Mac_Scene.exe as the startup item, then press Ctrl+F5.

## Controls

W, A, S, D move. Q and E go up and down. Mouse looks around. Scroll changes speed. P is perspective, O is orthographic. ESC exits.

## Layout

- Source files are in the project root.
- Libraries/ holds GLEW, GLFW, and GLM so the project does not depend on any outside folder.