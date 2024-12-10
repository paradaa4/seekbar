include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(add_unit_test name)
  add_executable(${name}-test ${name}_unittest.cpp)
  target_link_libraries(${name}-test
    PUBLIC
      core
      gtest_main
  )
  add_test(NAME ${name}-test COMMAND ${name}-test)

endfunction()

add_unit_test(FilmController)
