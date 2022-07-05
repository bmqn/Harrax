cmake_minimum_required(VERSION 3.1.0)

set(DN_ROOT_DIR		${CMAKE_CURRENT_SOURCE_DIR})
set(DN_SRC_DIR		${DN_ROOT_DIR}/src)
set(DN_LIB_DIR		${DN_ROOT_DIR}/libs)

project(Dungeon)

#--------------------------------------------------------------------------------------------------
#	Configuration
#--------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#--------------------------------------------------------------------------------------------------
#	Sources
#--------------------------------------------------------------------------------------------------
set(DN_HSP
	"${DN_LIB_DIR}/glad/include/"
	"${DN_SRC_DIR}/"
)

set(DN_SRC
	"${DN_LIB_DIR}/glad/src/glad.c"

	"${DN_SRC_DIR}/Main.cpp"
	
	"${DN_SRC_DIR}/Config.h"
	
	"${DN_SRC_DIR}/util/Log.h"
	"${DN_SRC_DIR}/util/Time.hpp"
	"${DN_SRC_DIR}/util/Time.cpp"
	"${DN_SRC_DIR}/util/DynamicPool.hpp"

	"${DN_SRC_DIR}/maths/Algebra.hpp"
	
	"${DN_SRC_DIR}/app/App.hpp"
	"${DN_SRC_DIR}/app/App.cpp"
	"${DN_SRC_DIR}/app/Window.hpp"
	"${DN_SRC_DIR}/app/Window.cpp"

	"${DN_SRC_DIR}/graphics/Renderer.hpp"
	"${DN_SRC_DIR}/graphics/Renderer.cpp"

	"${DN_SRC_DIR}/game/Entity.hpp"
	"${DN_SRC_DIR}/game/Entity.cpp"
	"${DN_SRC_DIR}/game/Components.hpp"
)

#--------------------------------------------------------------------------------------------------
#	Libraries
#--------------------------------------------------------------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("libs/glfw")

add_subdirectory("libs/glm")

#--------------------------------------------------------------------------------------------------
#	Build
#--------------------------------------------------------------------------------------------------
add_executable(Dungeon ${DN_SRC})
target_include_directories(Dungeon PRIVATE ${DN_HSP})
target_link_libraries(Dungeon PRIVATE glfw glm)