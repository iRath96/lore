cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.2.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(lore-tests
  io/LensReader.cpp
)
target_link_libraries(lore-tests PRIVATE lore Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(lore-tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
