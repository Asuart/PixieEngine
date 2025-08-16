# About
This project contains a prototype of a game engine and is used for learning purposes. Currently it's in the process of rewriting to to be Render API agnostic with option to select from `OpenGL` and `Vulkan`.

The goal is to imlement Computer Graphics features in `PixieEngineCore` and demonstrate them in separate projects.

`PixieEngineUI` - is an `ImGui` based UI library based on `PixieEngineCore`. It's made to be optinal for other projects and isolates Render API specific code from user.

`PixieEngineEditor` - is supposed to be an instrument for loading, editing and saving scenes. But for now it's more of a sandbox.

## Build
`CMake` is used to build the project. This project should not be used as example of using `CMake` as I am still learning it myself. I use `Windows 10` system with `Visual Studio Community 2022`. Build is not tested for different systems.

This project uses a lot of dependencies. `CMake` is configured to fetch most of them, but `Vulkan SDK` must be already installed on your system.

Inteded way of building project:

1) Create `build` directory in the project root with `mkdir build`.
2) Go to created directory `cd build`.
3) Call CMake `cmake ..`.

## License
- UNLICENSE for this repository (see `UNLICENSE.txt` for more details)