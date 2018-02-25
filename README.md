# Tristeon
Tristeon is an open source game engine built from scratch in C++ by two game engineering students. The project uses MSVC-specific keywords thus can only be compiled using the MSVC compiler. 

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
You will need visual studio to build the project, we use specific MSVC keywords which can only be compiled using visual studio. The visual studio project is pushed within the project so you can simply open that to access the project.
1. If you do not have the Vulkan SDK installed you can get it here: https://vulkan.lunarg.com/
2. Currently non-editor builds don't work so make sure to select either Editor or EditorDebug https://puu.sh/yYkob/94b290228c.png
3. Make sure to compile it at x64 as we currently don't support compiling at x86

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

# Known bugs
* Asset browser deletion doesn't delete file
* Material changes don't always get applied
* Camera doesn't exist until pressing it (after scene load)
* New folders won't display until you reload the engine
* Material properties won't get applied properly with vectors
* The inspector breaks when there's multiple properties with the same name (like having multiple Vector3s)
* DebugDrawManager only ever uses the color from the last add...() call

# Showcase
Watch the showcase video here:
https://www.youtube.com/watch?v=6kVTGHOJu-E
![Screenshot of the showcase video](https://i.imgur.com/R6GoWQ6.png)

# Engine architecture
<img src="Documents/Engine Architecture/Tristeon Core Architecture.png">

# Authors
Leon Brands - Engine/graphics programmer (https://leonbrandsprogrammer.wordpress.com/)

Tristan Metz - Engine tool programmer (https://tristanmetz.software/)

# Contribute
Don't hesitate to contribute. You can contribute to the development of Tristeon by:
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

Other resources:
* [Vulkan tutorial by Alexander Overvoorde](https://vulkan-tutorial.com/)
* [Vulkan examples by Sascha Willems](https://github.com/SaschaWillems/Vulkan)
