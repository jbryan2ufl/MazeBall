cmake_minimum_required(VERSION 3.11)

# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
# endif()

project(main)


# Include GLFW using FetchContent
include(FetchContent)
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
	FetchContent_Populate(glfw)
	set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
	set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
	set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
	set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
	add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Include Glad using FetchContent
FetchContent_Declare(
	glad
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
	FetchContent_Populate(glad)
	set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
	set(GLAD_API "gl=" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest")
	set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
	add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})
endif()

# Include GLM using FetchContent
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

# Include ImGui using FetchContent
FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
endif()

file(GLOB SOURCES "src/*.cpp")
file(GLOB IMGUI "${imgui_SOURCE_DIR}/*.cpp")

# Add executable
add_executable(main
	${SOURCES}
	${IMGUI}
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(main PRIVATE
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

target_sources(main PRIVATE
	${SOURCES}
)

target_link_libraries(main glfw glad glm)

# Set C++ standard
set_target_properties(main PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

target_compile_options(main PUBLIC -g)