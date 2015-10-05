#
#
#
#

set(PROJECT_NAME "liblb")

cmake_minimum_required(VERSION 2.8)
project(${PROJECT_NAME})

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
pkg_search_module(LIBUSP REQUIRED libusp)
find_library(M_LIB m)
find_package(Threads REQUIRED)

enable_testing()

set(LIBLB_LIB "lb")
set(LIBLB_INCLUDE "${PROJECT_SOURCE_DIR}/include")
set(LIBLB_SRC "${PROJECT_SOURCE_DIR}/src")
set(LIBLB_TEST "${PROJECT_SOURCE_DIR}/test")
set(LIBLB_VERSION_MAJOR "0")
set(LIBLB_VERSION_MINOR "0")
set(LIBLB_VERSION_PATCH "1")
set(LIBLB_VERSION
  "${LIBLB_VERSION_MAJOR}.${LIBLB_VERSION_MINOR}.${LIBLB_VERSION_PATCH}")

# Uncomment to make makefiles verbose
# set(CMAKE_VERBOSE_MAKEFILE ON)

# Compiler Options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG -O3")

# Source
add_subdirectory(${LIBLB_SRC})
add_subdirectory(${LIBLB_INCLUDE})

# Includes
include_directories(${LIBLB_INCLUDE})
include_directories(${UDEV_INCLUDE_DIRS})

# Library
add_library(${LIBLB_LIB} SHARED ${SOURCE_FILES})
target_link_libraries(${LIBLB_LIB} ${LIBUSP_LIBRARIES}
  ${M_LIB} ${CMAKE_THREAD_LIBS_INIT})

# Tests
add_subdirectory(${LIBLB_TEST})

# Installation
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

# Config File Settings
set(LIBLB_INSTALL_PREFIX      "${CMAKE_INSTALL_PREFIX}")
set(LIBLB_INSTALL_EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(LIBLB_INSTALL_LIBDIR
  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(LIBLB_INSTALL_INCLUDEDIR
  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

# Configure Files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/liblb.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/liblb.pc @ONLY)

# pkg-config
set(PKGCONFIG_INSTALL_DIR lib/pkgconfig CACHE PATH
  "Installation directory for pkg-config files")

# Set install targets
install(TARGETS ${LIBLB_LIB}
  DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADER_FILES}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liblb.pc
  DESTINATION "${PKGCONFIG_INSTALL_DIR}")
