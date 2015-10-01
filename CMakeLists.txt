#
#
#
#

set(PROJECT_NAME "liblc")

cmake_minimum_required(VERSION 2.8)
project(${PROJECT_NAME})

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
pkg_search_module(LIBUSP REQUIRED libusp)
find_library(M_LIB m)

enable_testing()

set(LIBLC_LIB "lc")
set(LIBLC_INCLUDE "${PROJECT_SOURCE_DIR}/include")
set(LIBLC_SRC "${PROJECT_SOURCE_DIR}/src")
set(LIBLC_TEST "${PROJECT_SOURCE_DIR}/test")
set(LIBLC_VERSION_MAJOR "0")
set(LIBLC_VERSION_MINOR "0")
set(LIBLC_VERSION_PATCH "1")
set(LIBLC_VERSION
  "${LIBLC_VERSION_MAJOR}.${LIBLC_VERSION_MINOR}.${LIBLC_VERSION_PATCH}")

# Uncomment to make makefiles verbose
# set(CMAKE_VERBOSE_MAKEFILE ON)

# Compiler Options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG -O3")

# Source
add_subdirectory(${LIBLC_SRC})
add_subdirectory(${LIBLC_INCLUDE})

# Includes
include_directories(${LIBLC_INCLUDE})
include_directories(${UDEV_INCLUDE_DIRS})

# Library
add_library(${LIBLC_LIB} SHARED ${SOURCE_FILES})
target_link_libraries(${LIBLC_LIB} ${LIBUSP_LIBRARIES} ${M_LIB})

# Tests
add_subdirectory(${LIBLC_TEST})

# Installation
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

# Config File Settings
set(LIBLC_INSTALL_PREFIX      "${CMAKE_INSTALL_PREFIX}")
set(LIBLC_INSTALL_EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(LIBLC_INSTALL_LIBDIR
  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(LIBLC_INSTALL_INCLUDEDIR
  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# Configure Files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/liblc.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/liblc.pc @ONLY)

# pkg-config
set(PKGCONFIG_INSTALL_DIR lib/pkgconfig CACHE PATH
  "Installation directory for pkg-config files")

# Set install targets
install(TARGETS ${LIBLC_LIB}
  DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADER_FILES}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liblc.pc
  DESTINATION "${PKGCONFIG_INSTALL_DIR}")
