# Tristeon 3D
Tristeon 3D is an open source game engine built from scratch in C++ by two game engineering students.

This project is now archived as it is no longer in active development in favor of https://github.com/tristeon/tristeon.

![screenshot of the editor](https://i.imgur.com/T663Nb6.png)

# Features
* Serialization
  * In-house type introspection
  * Scenes/Objects stored as json objects
  * Simple data (de)serialization
* Rendering
  * Forward rendering
  * Vulkan backend
  * Procedural shader properties
  * PBR Lighting
  * Albedo, AO, Metallic and roughness mapping
  * 3D Mesh rendering
  * Visual debugging tools
  * Skybox rendering
* Editor
  * Asset browser
  * Scene editor
  * GameObject hierarchy
  * Object inspector
* Scene management
  * Saving
  * Loading
  * Editing
* Input
  * Mouse/keyboard support
  * Built in key/button mapping

# Getting started
You wish to try out Tristeon 3D? Great! Here's two ways to get started:

## Pre-built
If you're just looking to try out Tristeon's current features without diving too much into the C++ side, then it'd be a good idea to go to the latest release: https://github.com/Tristeon/Tristeon3D/releases/tag/v1.0.0-pre-alpha

## CMake project
Tristeon3D uses CMake to configure the project. Please configure and build the project according to the CMake guidelines. We recommend to place the build folder outside of the repository as none of the IDE/compiler specific information needs to be shared across clients.
Officially confirmed configurations:
* MSVC Visual Studio 2017 | x64
* GCC MingW-w64 | x64

If you do not hav the Vulkan SDK installed, make sure to install it to allow for additional debugging. You can get it here: https://vulkan.lunarg.com/

To run the project, select your desired build configuration:
- Debug: Build (no editor) with debug symbols.
- Release: Build (no editor) without debug symbols, optimized.
- DebugEditor: Editor application with debug symbols.
- Editor: Editor application without debug symbols, but with logging.

# Why this project?
Tristeon is a hobby/learning/portfolio project of Tristan Metz and Leon Brands. The project was a 5 month school project with a focus on extending/improving our engine development skills.

# Showcase
Watch the showcase video here:
https://www.youtube.com/watch?v=6kVTGHOJu-E
![Screenshot of the showcase video](https://i.imgur.com/R6GoWQ6.png)

# Authors
Leon Brands - Engine/graphics programmer (https://leonbrandsprogrammer.wordpress.com/)
Tristan Metz - Engine tool programmer (https://tristanmetz.software/)

# Credits

Tristeon uses the following third-party libraries:
* [ImGui](https://github.com/ocornut/imgui)
* [Assimp](https://github.com/assimp/assimp)
* [glm](https://github.com/g-truc/glm)
* [GLFW](https://github.com/glfw/glfw)
* [stb](https://github.com/nothings/stb)
* [Vulkan](https://github.com/KhronosGroup/Vulkan-Hpp)
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
* [Boost](https://www.boost.org/doc/libs/1_67_0/libs/filesystem/doc/index.htm)
* [Boost-cmake](https://github.com/Orphis/boost-cmake)
