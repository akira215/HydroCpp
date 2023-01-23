cmake_minimum_required(VERSION 3.0.0)
project(HydroCpp VERSION 0.1.1)

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
option(OPENXLSX_BUILD_BENCHMARKS "Build and run library benchmarks" ON)
set(OPENXLSX_BUILD_BENCHMARKS OFF)

add_subdirectory(OpenXLSX)
add_subdirectory(nfd)

message(STATUS "OPENXLSX_BUILD_BENCHMARKS: ${OPENXLSX_BUILD_BENCHMARKS}")
message(STATUS "OPENXLSX_ENABLE_LIBZIP: ${OPENXLSX_ENABLE_LIBZIP}")
#======================================================================
# Source files
#======================================================================
file(GLOB CPP_SRC CONFIGURE_DEPENDS "src/*.cpp")

#======================================================================
# Find required libtraries
#======================================================================
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#======================================================================
# executable
#======================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

add_executable(${PROJECT_NAME} ${CPP_SRC})
target_link_libraries(${PROJECT_NAME} OpenXLSX::OpenXLSX nfd -static-libgcc -static-libstdc++ Threads::Threads)
#target_link_libraries (${PROJECT_NAME} OpenXLSX::OpenXLSX nfd -static gcc stdc++ winpthread -dynamic)

target_include_directories(OpenXLSX
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/include>)     

#======================================================================
# Creating installer (require NSIS)
#======================================================================
install(TARGETS ${PROJECT_NAME} DESTINATION bin )

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "bin")
set(CPACK_GENERATOR "NSIS")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_NAME "HydroCpp")
set(CPACK_PACKAGE_VENDOR "Akira Corp.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hydrostatics and KN computations")
set (CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PROJECT_NAME})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_IGNORE_FILES})

#set(CPACK_NSIS_INSTALLED_ICON_NAME "${ABC_SOURCE_DIR}/assets/abc.ico")
#set(CPACK_NSIS_HELP_LINK ${APP_URL})
#set(CPACK_NSIS_URL_INFO_ABOUT ${APP_URL})
#set(CPACK_NSIS_CONTACT ${APP_EMAIL})
#set(CPACK_NSIS_CREATE_ICONS_EXTRA
#"CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\abc.lnk' '$INSTDIR\\\\bin\\\\abc.exe' "
#)

# Installer icon
#set(CPACK_NSIS_MUI_ICON "${ABC_SOURCE_DIR}/assets/abc.ico")

set(CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/assets/nsis ${CMAKE_MODULE_PATH})
include(CPack)

configure_file(
    ${PROJECT_SOURCE_DIR}/resources/nsis/NSIS.definitions.nsh.in
    ${CMAKE_CURRENT_BINARY_DIR}/resources/nsis/NSIS.definitions.nsh
    )