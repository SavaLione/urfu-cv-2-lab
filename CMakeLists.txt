cmake_minimum_required (VERSION 3.13.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(LYSSA_PROJECT "lyssa")
set(LYSSA_PROJECT_VERSION "0.0.1")

project(${LYSSA_PROJECT}
    VERSION ${LYSSA_PROJECT_VERSION}
    LANGUAGES CXX
)

# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -fopenmp")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -DNDEBUG -fopenmp")
else()
    message(STATUS "Unknown build type: " ${CMAKE_BUILD_TYPE})
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Output directory function
function(function_output_directory arg_project)
    set_target_properties(${arg_project}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # lib
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" # lib
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction(function_output_directory)

# Libraries dependencies
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/imgui)
find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)
find_package(spdlog REQUIRED)

# Lyssa source files
set(LYSSA_SRC
   src/core/lyssa.h
   src/core/lyssa.cpp
)

# Lyssa executable
add_executable(${PROJECT_NAME} ${LYSSA_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# Threads
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# OpenGL
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})

# imgui
target_link_libraries(${PROJECT_NAME} imgui::imgui)

# OpenCV
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})

# SDL2
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})

# spdlog
target_link_libraries(${PROJECT_NAME} ${SPDLOG_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_INCLUDE_DIR})

# Output directories
function_output_directory(imgui)
function_output_directory(${PROJECT_NAME})
