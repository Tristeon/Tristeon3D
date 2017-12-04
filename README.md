# Tristeon
Tristeon is an open source game engine built from scratch by two game engineering students.

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
  * PBR Shading (In development)
  * Shader-Material system
  * Diffuse textures
  * 3D Mesh rendering
  * Camera component based rendering
  * Visual debugging tools
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
* Mesh Loading
  * Vertices
  * UVs
  * Normals
  * Over 40 file formats including FBX, OBJ, 3DS and blend
* Image Loading
  * Formats: JPG, PNG, TGA, BMP, PSD and more

# Roadmap

Near future:
* PBR with textures and image based lighting
* Point/Spot/Directional lights
* Flexible shader property system
* Shadow mapping
* Normal and emission maps

Planned:
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
* In-Editor Vulkan shader compilation

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
