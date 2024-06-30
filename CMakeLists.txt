# CMakeList.txt : CMake project for Renderella, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Renderella")

add_subdirectory(external/glfw)
add_subdirectory(external/glm/glm)

include_directories(
	src
	external/glfw/include
	external/glm/glm
	external/glad/include
	external/stb
)

file(GLOB_RECURSE SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"src/*.cpp"
	"external/glad/src/glad.c"
)

set(COMMON_SOURCES ${SOURCES})

set(LIBS
	glfw
	glm
)

set(COMMON_LIBS ${LIBS})

add_subdirectory("demo/sponza")
add_subdirectory("demo/terrain")

