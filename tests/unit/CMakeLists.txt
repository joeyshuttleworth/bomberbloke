include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests run_and_close.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain CLIENT)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)
