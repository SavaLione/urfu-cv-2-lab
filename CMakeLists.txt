cmake_minimum_required (VERSION 3.13.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(lyssa
    VERSION "0.0.1"
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
find_package(SDL2_image REQUIRED)
find_package(spdlog REQUIRED)
#find_package(GLI REQUIRED)
#set(GLI_TEST_ENABLE OFF)
#add_subdirectory(${PROJECT_SOURCE_DIR}/lib/gli)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glm)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/stb-cmake)
find_package(GLEW REQUIRED)
#find_package(fmt REQUIRED)

# Lyssa source files
set(LYSSA_SRC
   src/core/lyssa.h
   src/core/lyssa.cpp
   src/core/shader.h
   src/core/shader.cpp
)

# Lyssa executable
add_executable(${PROJECT_NAME} ${LYSSA_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

# Libs
target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} imgui::imgui)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SPDLOG_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/gli)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} stb_image)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
#target_link_libraries(${PROJECT_NAME} fmt::fmt)

# Output directories
function_output_directory(imgui)
function_output_directory(${PROJECT_NAME})
