# Tristeon
Tristeon is an open source game engine built from scratch in C++ by two game engineering students.

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

# How to build the project
Tristeon uses CMake to configure the project. Please configure and build the project according to the CMake guidelines. We recommend to place the build folder outside of the repository as none of the IDE/compiler specific information needs to be shared across clients.
Officially confirmed configurations:
* MSVC Visual Studio 2017 | x64
* GCC MingW-w64 | x64

If you do not hav the Vulkan SDK installed, make sure to install it to allow for additional debugging. You can get it here: https://vulkan.lunarg.com/

# Why?
Tristeon is a learning / portfolio project of Tristan Metz and Leon Brands. The project started as a 5 month school project with as focus to extend/improve our engine development skills and knowledge. As of now, it's being maintained and updated on an irregular basis as a portfolio piece.

# Roadmap
Near future:
* Image based lighting
* Point/Spot/Directional lights
* Shadow mapping
* Optimized component serialization
* Dragging/dropping (file)items

Planned (editor/serialization):
* Automized serialization
* In-Editor console logging
* Scene view object selection
* Renaming items
* Serialization wrappers for ease of use

Planned (rendering/engine):
* Expand support for material property types
* Optimized DebugDrawManager
* Redesigned rendering back-end
* Preview renderer for materials/meshes
* Normal mapping
* Image effect system
* Havok physics engine integration
* FMOD Audio integration

# Known bugs
* ~~#1 Scene view won't display properly until the engine's game mode has been run at least once.~~
* #2 Asset browser deletion doesn't delete file
* ~~#3 Material changes don't always get applied~~
* ~~#4 Camera doesn't exist until pressing it (after scene load)~~
* #5 New folders won't display until you reload the engine
* #6 Material properties won't get applied properly with vectors
* #7 The inspector breaks when there's multiple properties with the same name (like having multiple Vector3s, their subcomponents are called the same)
* #8 DebugDrawManager only ever uses the color from the last add...() call

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

Other resources:
* [Vulkan tutorial by Alexander Overvoorde](https://vulkan-tutorial.com/)
* [Vulkan examples by Sascha Willems](https://github.com/SaschaWillems/Vulkan)
