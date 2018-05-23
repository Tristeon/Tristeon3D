cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

#Project
project(Tristeon)
set (Tristeon_VERSION_MAJOR 1)
set (Tristeon_VERSION_MINOR 1)	
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
include_directories("${PROJECT_SOURCE_DIR}/include")
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


#Config definitions
if (MSVC)
set(EDITOR_DEF -DTRISTEON_EDITOR=1)
set(LOG_DEF -DTRISTEON_LOGENABLED=1)
else(MSVC)
set(EDITOR_DEF -DTRISTEON_EDITOR)
set(LOG_DEF -DTRISTEON_LOGENABLED)
endif(MSVC)

set(CMAKE_CXX_FLAGS_DEBUGEDITOR "${EDITOR_DEF} ${LOG_DEF}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUGEDITOR ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_DEBUGEDITOR ${CMAKE_C_FLAGS_DEBUG})

set(CMAKE_CXX_FLAGS_EDITOR "${EDITOR_DEF} ${LOG_DEF}")
set(CMAKE_EXE_LINKER_FLAGS_EDITOR ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
set(CMAKE_C_FLAGS_EDITOR ${CMAKE_C_FLAGS_RELEASE})

set(CMAKE_CXX_FLAGS_DEBUG "${LOG_DEF}")

set(CMAKE_CONFIGURATION_TYPES Debug DebugEditor Release Editor)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
	
#Vulkan
IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			set(VULKAN_LIBRARY ${Vulkan_LIBRARY})
		ENDIF()
	ENDIF()
ELSE(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			set(VULKAN_LIBRARY ${Vulkan_LIBRARY})
		ENDIF()
	endif()
endif()
IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ENDIF()
link_libraries(${Vulkan_LIBRARY})

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
add_subdirectory(external/gli)
include_directories(${PROJECT_SOURCE_DIR}/external/gli)
include_directories(${PROJECT_BINARY_DIR}/external/gli)

#SPIRV-Cross
add_subdirectory(external/spirv_cross)

include_directories(${PROJECT_SOURCE_DIR}/external/spirv_cross/include)
include_directories(${PROJECT_BINARY_DIR}/external/spirv_cross/include)

#Boost
set(BOOST_URL "")
set(BOOST_SOURCE ${PROJECT_SOURCE_DIR}/external/boost)
add_subdirectory(external/boost-cmake)

#Libraries
macro(link_libs targetname)
	target_link_libraries(${targetname} glfw)
	target_link_libraries(${targetname} assimp)
	target_link_libraries(${targetname} gli)
	target_link_libraries(${targetname} glm)
	target_link_libraries(${targetname} Boost::filesystem)
	
	target_link_libraries(${targetname} spirv-cross-core)
	target_link_libraries(${targetname} spirv-cross-cpp)
	target_link_libraries(${targetname} spirv-cross-glsl)
	target_link_libraries(${targetname} spirv-cross-hlsl)
	target_link_libraries(${targetname} spirv-cross-msl)
endmacro()

if (MSVC)
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