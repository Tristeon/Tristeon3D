cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

#Project
project(Tristeon)
set (Tristeon_VERSION_MAJOR 1)
set (Tristeon_VERSION_MINOR 1)	
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Output/compilation
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
include_directories("${PROJECT_SOURCE_DIR}/src")

#Add source files
macro(read_directory curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	
	set(groupList "")
	
	foreach(child ${children})
		if (NOT (IS_DIRECTORY ${curdir}/${child}))
			list(APPEND tristeonSRC ${curdir}/${child})
			list(APPEND groupList ${curdir}/${child})
		endif()
	endforeach()
	
	list(LENGTH groupList length)
	if (length GREATER 0)
		string(REPLACE ${PROJECT_SOURCE_DIR} "" GROUPNAME ${curdir})
		string(REPLACE "/" "\\" GROUPNAME ${GROUPNAME})
		source_group(${GROUPNAME} FILES ${groupList})
	endif()
	
	foreach(child ${children})
		if (IS_DIRECTORY ${curdir}/${child})
			read_directory(${curdir}/${child})
		endif()
	endforeach()
endmacro()
read_directory(${PROJECT_SOURCE_DIR}/src)

#Check if submodules are downloaded
if(	NOT EXISTS "${PROJECT_SOURCE_DIR}/external/assimp/CMakeLists.txt" OR
NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt" OR
NOT EXISTS "${PROJECT_SOURCE_DIR}/external/gli/CMakeLists.txt" OR
NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glm/CMakeLists.txt")
	message(STATUS "The git submodules were not updated, attempting to do so now")
	find_package(Git)
	if (NOT GIT_FOUND)
		message(FATAL_ERROR "Couldn't find Git executable thus the git submodules weren't downloaded. To build Tristeon, you'll need to download git and try again, or run git submodule update --init --recursive yourself.")
	else()
		message(STATUS "Git found, updating submodules")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
		endif()
	endif()
endif()

#Config definitions
if (MSVC)
set(EDITOR_DEF -DTRISTEON_EDITOR=1)
set(LOG_DEF -DTRISTEON_LOGENABLED=1)
set(DEBUG_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
else(MSVC)
set(EDITOR_DEF -DTRISTEON_EDITOR)
set(LOG_DEF -DTRISTEON_LOGENABLED)
set(DEBUG_FLAGS )
endif(MSVC)

set(CMAKE_CXX_FLAGS_DEBUGEDITOR "${DEBUG_FLAGS} ${EDITOR_DEF} ${LOG_DEF}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGEDITOR ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
set(CMAKE_SHARED_LINKER_FLAGS_DEBUGEDITOR ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_DEBUGEDITOR ${CMAKE_C_FLAGS_DEBUG})

set(CMAKE_CXX_FLAGS_EDITOR "${EDITOR_DEF} ${LOG_DEF}")
set(CMAKE_EXE_LINKER_FLAGS_EDITOR ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
set(CMAKE_SHARED_LINKER_FLAGS_EDITOR ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
set(CMAKE_C_FLAGS_EDITOR ${CMAKE_C_FLAGS_RELEASE})

set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG_FLAGS} ${LOG_DEF}")

set(CMAKE_CONFIGURATION_TYPES Debug DebugEditor Release Editor)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
	
#Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)
include_directories(${Vulkan_INCLUDE_DIRS})

#GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

#ASSIMP
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)
add_subdirectory(external/assimp)
include_directories(${PROJECT_SOURCE_DIR}/external/assimp/include)
include_directories(${PROJECT_BINARY_DIR}/external/assimp/include)

#GLM
add_subdirectory(external/glm)

#GLI
set(GLI_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(external/gli)
include_directories(${PROJECT_SOURCE_DIR}/external/gli)
include_directories(${PROJECT_BINARY_DIR}/external/gli)

#header only 
include_directories(${PROJECT_SOURCE_DIR}/external/stb)
include_directories(${PROJECT_SOURCE_DIR}/external/rlutil)
include_directories(${PROJECT_SOURCE_DIR}/external/json)

#Libraries
macro(link_libs targetname)
	target_link_libraries(${targetname} glfw)
	target_link_libraries(${targetname} assimp)
	target_link_libraries(${targetname} gli)
	target_link_libraries(${targetname} glm)
	
	target_link_libraries(${targetname} ${Vulkan_LIBRARIES})
endmacro()

#Compile shader function
function(add_shader TARGET SHADER)
    find_program(GLSLC glslc)

    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/bin/Files/Shaders/${SHADER})
    set(current-output-path ${CMAKE_CURRENT_SOURCE_DIR}/bin/Files/Shaders/${SHADER}.spv)

    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
           OUTPUT ${current-output-path}
           COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
           DEPENDS ${current-shader-path}
           IMPLICIT_DEPENDS CXX ${current-shader-path}
           VERBATIM)

    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)

	set_property(TARGET ${TARGET} APPEND PROPERTY SOURCES ${current-output-path})
	set_property(TARGET ${TARGET} APPEND PROPERTY SOURCES ${current-shader-path})
endfunction(add_shader)

#Add shaders as a custom target
add_custom_target(Shaders)
file(GLOB_RECURSE shader_files RELATIVE ${PROJECT_SOURCE_DIR}/bin/Files/Shaders/ ${PROJECT_SOURCE_DIR}/bin/*.vert ${PROJECT_SOURCE_DIR}/bin/*.frag)
foreach(child ${shader_files})
	add_shader(Shaders ${child})
endforeach()

if (MSVC)
	target_compile_options(glfw INTERFACE /MP)
	target_compile_options(assimp INTERFACE /MP)
	target_compile_options(gli INTERFACE /MP)
	target_compile_options(glm INTERFACE /MP)
	
	add_definitions(/MP)
	
	add_executable(Tristeon ${tristeonSRC})
	
	#Set target output directory
	set_target_properties(Tristeon
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
		
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
	
		ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITOR "${CMAKE_SOURCE_DIR}/bin"
		LIBRARY_OUTPUT_DIRECTORY_DEBUGEDITOR "${CMAKE_SOURCE_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUGEDITOR "${CMAKE_SOURCE_DIR}/bin"
	
		ARCHIVE_OUTPUT_DIRECTORY_EDITOR "${CMAKE_SOURCE_DIR}/bin"
		LIBRARY_OUTPUT_DIRECTORY_EDITOR "${CMAKE_SOURCE_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY_EDITOR "${CMAKE_SOURCE_DIR}/bin"
	)
	
	#Set working directory to build in MSVC
	file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/Tristeon.vcxproj.user" 
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>     \
		<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
		<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		</PropertyGroup>
		<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		</PropertyGroup>
		<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='DebugEditor|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		</PropertyGroup>
		<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Editor|x64'\">
			<LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
			<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
		</PropertyGroup>
		</Project>"
	)
	#Set Tristeon as the start project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Tristeon)

	link_libs(Tristeon)
	
else(MSVC)
	add_executable(Debug ${tristeonSRC})
	add_executable(Release ${tristeonSRC})
	add_executable(Editor ${tristeonSRC})
	add_executable(DebugEditor ${tristeonSRC})
	
	target_compile_definitions(Debug PUBLIC ${CMAKE_CXX_FLAGS_DEBUG})
	target_compile_definitions(Release PUBLIC ${CMAKE_CXX_FLAGS_RELEASE})
	target_compile_definitions(Editor PUBLIC ${CMAKE_CXX_FLAGS_DEBUGEDITOR})
	target_compile_definitions(DebugEditor PUBLIC ${CMAKE_CXX_FLAGS_EDITOR})
	
	link_libs(Debug)
	link_libs(Release)
	link_libs(Editor)
	link_libs(DebugEditor)
	
endif(MSVC)

add_dependencies(Tristeon Shaders)