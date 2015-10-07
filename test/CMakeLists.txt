include(FindPkgConfig)
pkg_check_modules(CHECK REQUIRED check)

include_directories(${CHECK_INCLUDE_DIRS})

file(GLOB TEST_SOURCE_FILES "*.c")

foreach(CURRENT_TEST_SOURCE_FILE ${TEST_SOURCE_FILES})
  get_filename_component(CURRENT_TEST_BINARY ${CURRENT_TEST_SOURCE_FILE} NAME_WE)
  string(REPLACE "test_" "" CURRENT_TEST ${CURRENT_TEST_BINARY})

  add_executable(${CURRENT_TEST_BINARY} ${CURRENT_TEST_SOURCE_FILE})
  target_link_libraries(${CURRENT_TEST_BINARY} ${LIBLB_LIB} ${CHECK_LIBRARIES} pthread)
  target_include_directories(${CURRENT_TEST_BINARY} PUBLIC ${LIBLB_INCLUDE})

  add_test(${CURRENT_TEST} ${CURRENT_TEST_BINARY})
endforeach()
