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

file(GLOB_RECURSE SOURCES RELATIVE 
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"src/*.cpp"
	"external/glad/src/glad.c"
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME}  ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# Copy assets to build directory
add_custom_target(copy_assets ALL
  COMMENT "Copying assets to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/assets
          ${CMAKE_CURRENT_BINARY_DIR}/assets
  DEPENDS ${PROJECT_NAME}
)
