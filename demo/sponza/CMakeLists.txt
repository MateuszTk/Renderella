# Add source to this project's executable.
add_executable (sponza main.cpp ${COMMON_SOURCES})

target_link_libraries(sponza ${COMMON_LIBS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sponza PROPERTY CXX_STANDARD 20)
endif()

# Copy assets to build directory
add_custom_target(copy_assets ALL
  COMMENT "Copying assets to build directory"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/../../assets
          ${CMAKE_CURRENT_BINARY_DIR}/assets
  DEPENDS sponza
)

