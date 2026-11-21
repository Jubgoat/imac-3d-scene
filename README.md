\# iMac 3D Scene



A 3D scene of an iMac on a desk, built with C++ and OpenGL for my computational graphics and visualization course at SNHU. I picked the iMac because it is made of simple shapes (a flat screen, a thin stand, a base) that I could build from basic meshes and then texture and light to look like a real object.



\## What the Scene Contains



The scene is built from basic 3D shapes combined and transformed to make the iMac, the desk it sits on, and a few objects around it. Each object has its own texture and material, and the scene uses more than one light source so the surfaces show highlights and shadows instead of looking flat.



\## Controls



The camera can be moved around the scene with the keyboard and mouse. WASD moves the camera, Q and E move it up and down, the mouse changes where it looks, and the scroll wheel changes the movement speed. Pressing P switches between perspective and orthographic views. Pressing Esc closes the program.



\## Folder Layout



The Imac folder holds the final scene. The Project one folder holds my earlier work from the first project. ModuleOne.docx is the Module One assignment document.



\## How to Build and Run



I built this in Visual Studio on Windows. To run it, clone the repository, open the solution file inside the Imac folder, set the build configuration to Debug or Release with x64, and press Ctrl+F5. The project expects GLFW, GLEW, GLM, and the stb\_image library to already be linked, which they are in the project settings. The texture images need to stay in the same folder structure they are in now, or the objects will show up untextured.



```

git clone https://github.com/Jubgoat/imac-3d-scene.git

```



\## Design Decisions



I kept the geometry simple and let the textures and lighting do most of the work. Breaking the iMac into a few basic shapes made it easier to scale, rotate, and position each part correctly, and it kept the code organized because each object follows the same steps of loading its texture, setting its material, and drawing it.



\## Author



Zack Crostreet, Southern New Hampshire University

