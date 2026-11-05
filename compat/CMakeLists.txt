# Included AFTER the existing add_subdirectory(quadwild) in root CMakeLists.txt.
if(NOT WIN32)
  message(FATAL_ERROR "This adapter package targets Windows DLLs")
endif()

add_library(lib_quadwild SHARED quadwild_bridge.cpp ../quadwild/trace.cpp)
target_include_directories(lib_quadwild PRIVATE "${PROJECT_SOURCE_DIR}/quadwild")
target_link_libraries(lib_quadwild PRIVATE
  quadwild::quadretopology
  quadwild::lib_field_computation
  quadwild::xfield_tracer
  quadwild::quad_from_patches)

add_library(lib_quadpatches SHARED quadpatches_bridge.cpp)
target_link_libraries(lib_quadpatches PRIVATE
  quadwild::quad_from_patches
  nlohmann_json::nlohmann_json)

if(MSVC)
  target_link_options(lib_quadwild PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/lib_quadwild.def")
  target_link_options(lib_quadpatches PRIVATE "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/lib_quadpatches.def")
else()
  target_sources(lib_quadwild PRIVATE lib_quadwild.def)
  target_sources(lib_quadpatches PRIVATE lib_quadpatches.def)
endif()

foreach(tgt IN ITEMS lib_quadwild lib_quadpatches)
  set_target_properties(${tgt} PROPERTIES
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qremeshify_dll"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qremeshify_dll")
endforeach()
