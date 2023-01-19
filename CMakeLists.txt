cmake_minimum_required(VERSION 3.0.0)
project(HydroCpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

#======================================================================
# Set the build output location to a common directory
#======================================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

#======================================================================
# Subdirectories
#======================================================================
add_subdirectory(OpenXLSX)
add_subdirectory(nfd)


#======================================================================
# Source files
#======================================================================
file(GLOB CPP_SRC CONFIGURE_DEPENDS "src/*.cpp")


#======================================================================
# executable
#======================================================================
add_executable(${PROJECT_NAME} ${CPP_SRC})
target_link_libraries(${PROJECT_NAME} OpenXLSX::OpenXLSX nfd -static-libgcc -static-libstdc++)

target_include_directories(OpenXLSX
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/include>)     


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
