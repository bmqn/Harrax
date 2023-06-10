cmake_minimum_required(VERSION 3.1.0)

set(DN_ROOT_DIR		${CMAKE_CURRENT_SOURCE_DIR})
set(DN_SRC_DIR		${DN_ROOT_DIR}/src)
set(DN_LIB_DIR		${DN_ROOT_DIR}/libs)
set(DN_RES_DIR		${DN_ROOT_DIR}/res)

project(Harrax)

#--------------------------------------------------------------------------------------------------
#	Configuration
#--------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(MSVC)
add_compile_options(
	$<$<CONFIG:RELEASE>:/Ox>
	$<$<CONFIG:RELEASE>:/GL>
)
add_link_options(
	$<$<CONFIG:RELEASE>:/LTCG>
)
else()
add_compile_options(
	$<$<CONFIG:RELEASE>:-Ofast>
)
add_link_options(
	$<$<CONFIG:RELEASE>:-flto>
)
endif()

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
	"${DN_SRC_DIR}/util/Random.hpp"
	"${DN_SRC_DIR}/util/Random.cpp"
	"${DN_SRC_DIR}/util/File.hpp"
	"${DN_SRC_DIR}/util/DynamicPool.hpp"

	"${DN_SRC_DIR}/maths/Algebra.hpp"
	
	"${DN_SRC_DIR}/app/App.hpp"
	"${DN_SRC_DIR}/app/App.cpp"
	"${DN_SRC_DIR}/app/Event.hpp"
	"${DN_SRC_DIR}/app/Window.hpp"
	"${DN_SRC_DIR}/app/Window.cpp"
	"${DN_SRC_DIR}/app/Input.hpp"
	"${DN_SRC_DIR}/app/Input.cpp"

	"${DN_SRC_DIR}/graphics/Camera.hpp"
	"${DN_SRC_DIR}/graphics/Renderer.hpp"
	"${DN_SRC_DIR}/graphics/Renderer.cpp"

	"${DN_SRC_DIR}/game/Registration.hpp"
	"${DN_SRC_DIR}/game/Entity.hpp"
	"${DN_SRC_DIR}/game/Registry.hpp"
	"${DN_SRC_DIR}/game/Particles.hpp"
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
add_executable(Harrax ${DN_SRC})
target_include_directories(Harrax PRIVATE ${DN_HSP})
target_link_libraries(Harrax PRIVATE glfw glm)

set_target_properties(Harrax PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Harrax>
)

#--------------------------------------------------------------------------------------------------
#	Resources
#--------------------------------------------------------------------------------------------------
add_custom_target(Harrax_CopyResources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DN_RES_DIR} $<TARGET_FILE_DIR:Harrax>
)