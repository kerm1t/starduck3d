# Starduck3d

3d engine for small prototyping projects e.g. simulators, games, etc.  
Uses OpenGL  
Windows  

Plans:
- Multiplatform
- bgfx
- IOS

HowTo:  
download glew, copy to /lib, build glew with Visual Studio (to build glew32sd.lib)  
adapt glew version CMAKELISTS.txt, i.e.
```
set(glew "glew-2.1.0")
```
then make the project
```
mkdir build
cd build
cmake ..
```
