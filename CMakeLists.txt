cmake_minimum_required(VERSION 3.16.3)

set(HR_TAREGT_GLFW False)
set(HR_TARGET_ANDROID True)

set(HR_ROOT_DIR		${CMAKE_CURRENT_SOURCE_DIR})
set(HR_SRC_DIR		${HR_ROOT_DIR}/src)
set(HR_LIB_DIR		${HR_ROOT_DIR}/libs)
set(HR_RES_DIR		${HR_ROOT_DIR}/res)

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
set(HR_HSP_SHARED
	"${HR_SRC_DIR}/"
)

if(HR_TAREGT_GLFW)

set(HR_HSP_TARGET
	"${HR_LIB_DIR}/glad/include/"
)

endif()

set(HR_HSP ${HR_HSP_TARGET} ${HR_HSP_SHARED})

set(HR_SRC_SHARED
	"${HR_SRC_DIR}/Config.h"
	
	"${HR_SRC_DIR}/util/Log.h"
	"${HR_SRC_DIR}/util/Time.hpp"
	"${HR_SRC_DIR}/util/Time.cpp"
	"${HR_SRC_DIR}/util/Random.hpp"
	"${HR_SRC_DIR}/util/Random.cpp"
	"${HR_SRC_DIR}/util/File.hpp"
	"${HR_SRC_DIR}/util/DynamicPool.hpp"

	"${HR_SRC_DIR}/maths/Algebra.hpp"
	
	"${HR_SRC_DIR}/app/App.hpp"
	"${HR_SRC_DIR}/app/App.cpp"
	"${HR_SRC_DIR}/app/Window.hpp"
	"${HR_SRC_DIR}/app/Input.hpp"

	"${HR_SRC_DIR}/graphics/Renderer.hpp"

	"${HR_SRC_DIR}/game/Registry.hpp"
	"${HR_SRC_DIR}/game/Entity.hpp"
	"${HR_SRC_DIR}/game/System.hpp"
	"${HR_SRC_DIR}/game/Registration.hpp"
)

if(HR_TAREGT_GLFW)

set(HR_SRC_TARGET
	"${HR_LIB_DIR}/glad/src/glad.c"
	
	"${HR_SRC_DIR}/target/glfw/Main.cpp"

	"${HR_SRC_DIR}/app/target/glfw/GLFWWindow.cpp"
	"${HR_SRC_DIR}/app/target/glfw/GLFWInput.cpp"
	"${HR_SRC_DIR}/graphics/target/glfw/GLFWRenderer.cpp"
)

elseif(HR_TARGET_ANDROID)

set(HR_SRC_TARGET
	"${HR_SRC_DIR}/target/android/app/src/main/cpp/Main.cpp"

	"${HR_SRC_DIR}/app/target/android/AndroidWindow.cpp"
	"${HR_SRC_DIR}/app/target/android/AndroidInput.cpp"
	"${HR_SRC_DIR}/graphics/target/android/AndroidRenderer.cpp"
)

endif()

set(HR_SRC ${HR_SRC_TARGET} ${HR_SRC_SHARED})

#--------------------------------------------------------------------------------------------------
#	Libraries
#--------------------------------------------------------------------------------------------------
if(HR_TAREGT_GLFW)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("libs/glfw")

endif()

add_subdirectory("libs/glm")

#--------------------------------------------------------------------------------------------------
#	Build
#--------------------------------------------------------------------------------------------------
if(HR_TAREGT_GLFW)

add_executable(harrax ${HR_SRC})
target_include_directories(harrax PRIVATE ${HR_HSP})
target_link_libraries(harrax PRIVATE
	glfw
	glm
)
target_compile_definitions(harrax PRIVATE HR_TARGET_GLFW=1)

set_target_properties(harrax PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:harrax>
)


elseif(HR_TARGET_ANDROID)

add_library(harrax SHARED ${HR_SRC})
target_include_directories(harrax PRIVATE ${HR_HSP})
target_link_libraries(harrax PRIVATE
	android
	log
	EGL
	GLESv2
	glm
)
target_compile_definitions(harrax PRIVATE HR_TARGET_ANDROID=1)

endif()

#--------------------------------------------------------------------------------------------------
#	Resources
#--------------------------------------------------------------------------------------------------
add_custom_target(Harrax_CopyResources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${HR_RES_DIR} $<TARGET_FILE_DIR:harrax>
)