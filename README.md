# Tristeon
Tristeon is an open source game engine built from scratch in C++ by two game engineering students. The project uses MSVC-specific keywords thus can only be compiled using the MSVC compiler. 

![screenshot of the editor](https://i.imgur.com/0CCvybQ.png)

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
  * PBR Lighting
  * Shader-Material system
  * Diffuse textures
  * 3D Mesh rendering
  * Camera component based rendering
  * Visual debugging tools
  * Procedural shader property system
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
  * Built in keymapping
* Utility
  * Vector2/3
  * Quaternion
  * Console
  * Properties
  * Delegates
  * vector
* Over 40 mesh file formats including FBX, OBJ, 3DS and blend
* Supported image formats: JPG, PNG, TGA, BMP, PSD and more

# Roadmap

Near future:
* Image based PBR
* Skybox rendering
* Point/Spot/Directional lights
* Shadow mapping
* Normal, emission, roughness, metallicness and AO maps

Planned:
* Automized serialization
* In-Editor console logging
* Image effect system including:
  * SSAO
  * Bloom
* Outline rendering for selected objects
* Pixel-perfect object selection
* Skinned mesh rendering
* Skeletal animations
* Anti-aliasing
* HDR
* Shadow cascades
* UI Rendering
* "Uber" shader compilation using spirv-cross

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
