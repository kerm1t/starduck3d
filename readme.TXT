5/24/2021
- build starduck for x64 (cmake .. -A x64)
  ... maybe starduck needs quite a lot of changes for a simple 3D devkit, let's try paulafast


9/20/2020
- FPS auf 60 begrenzen
  https://stackoverflow.com/questions/14968857/controlling-fps-limit-in-opengl-application


9/11/2020
How to build
1) get libraries ImGUI, Glew
2) build Glew
3) update CMakelists to reflect glew *.lib
4) build starduck



9/8/2020
Visual Studio 2019, X64
doesn't load glew32sd.lib anymore, but libglew32d.lib



2018-10-15
- Objekte als billboard einstellen



2018-09-16
- dies ist noch die alte Verzeichnisstruktur (Nebulus)
- die Libraries sind schon hochgezogen (Glew 2.0)
- Visual Studio Solution f�r VS2015

Das Projekt ist von irgendwo her und angepasst,
jedenfalls nicht mit einem Build Tool (CMake) generiert.

Das Laden der conferenceroom Datei (~26MB) dauert definitiv
zu lange auf meinem SONY. 2020-10-14, auf dem Razor auch