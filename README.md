# Tristeon 3D
Tristeon 3D is an open source game engine built from scratch in C++ by two game engineering students.

This project is now archived as it is no longer in active development in favor of https://github.com/tristeon/tristeon.

![screenshot of the editor](https://i.imgur.com/T663Nb6.png)

# Features
* Modular and clean engine framework
* Entity-component system
* Fully documented
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
* Over 40 mesh file formats including FBX, OBJ, 3DS and blend
* Supported image formats: JPG, PNG, TGA, BMP, PSD and more

# Getting started
You wish to try out Tristeon 3D? Great! Here's two ways to get started:

## Pre-built
If you're just looking to test out Tristeon's current features without diving too much into the C++ side, then it'd be a good idea to go to the latest release: https://github.com/Tristeon/Tristeon3D/releases/tag/v1.0.0-pre-alpha

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
Tristeon is a hobby/learning/portfolio project of Tristan Metz and Leon Brands. The project started as a 5 month school project with as focus to extend/improve our engine development skills and knowledge. As of now, it's being maintained and updated as a pure hobby and portfolio project.

# Roadmap
Near future:
* In-engine shader compilation
* Full shader reflection with complete type support
* Rendering framework redesign
* Debug draw manager optimizations + extended features
* Dragging/dropping (file)items into the inspector
* In-house physics engine implementation

Planned (editor/serialization):
* Optimized component serialization
* Automized serialization
* In-Editor console logging
* Scene view object selection
* Renaming items
* Serialization wrappers for ease of use
* Custom type editors

Planned (rendering/engine):
* Rework engine sub-systems into a module based system, allowing the user to add/remove modules at will
* Image based lighting
* Controllable point/spot/directional lights
* Shadow mapping
* Preview renderer for materials, meshes and images
* Normal mapping
* Image effect system

# Known bugs
* ~~#1 Scene view won't display properly until the engine's game mode has been run at least once.~~
* #2 Asset browser deletion doesn't delete file
* ~~#3 Material changes don't always get applied~~
* ~~#4 Camera doesn't exist until pressing it (after scene load)~~
* #5 New folders won't display until you reload the engine
* #6 Material properties won't get applied properly with vectors
* #7 The inspector breaks when there's multiple properties with the same name (e.g. having multiple Vector3s, their subcomponents are called the same)
* #8 DebugDrawManager only ever uses the color from the last add...() call
* #9 Editor doesn't recognize engine filetypes correctly when they haven't been created in-engine. Will default to AssetItem
* #10 Deleting a component also deletes each component below said component
* #11 The editor camera loses control at the minimum zoom distance
* #12 The editor camera may flip, causing controls to be inverted

# Showcase
Watch the showcase video here:
https://www.youtube.com/watch?v=6kVTGHOJu-E
![Screenshot of the showcase video](https://i.imgur.com/R6GoWQ6.png)

# Authors
Leon Brands - Engine/graphics programmer (https://leonbrandsprogrammer.wordpress.com/)

Tristan Metz - Engine tool programmer (https://tristanmetz.software/)

# Contribute
We'd love your help! You can contribute to the development of Tristeon by:
* Submitting an issue
* Submitting a pull request

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
