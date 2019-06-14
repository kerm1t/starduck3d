# Starduck3d

![alt text](https://github.com/kerm1t/starduck3d/blob/master/devlog/20190420_jeep_innertexture_bug_again.png)

3d engine for small prototyping projects e.g. simulators, games, etc.  
Uses OpenGL  
Windows  

Plans:
- Multiplatform
- bgfx
- IOS

HowTo:  
- download glew-SRC *), copy to /lib, build glew with Visual Studio (to build glew32sd.lib) ... *) binary only contains release mode libs 
- download ImGUI, copy to /lib
- patch ImGUI (doesn't detect mouse move currently)
```
static void ImGui_ImplWin32_UpdateMousePos()
[...]
//    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
[...]
```

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
