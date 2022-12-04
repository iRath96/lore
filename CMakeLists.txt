cmake_minimum_required(VERSION 3.11)

option(ENABLE_TESTS "Enable tests" ON)

project(lore
  DESCRIPTION
    "Lens core library"
  LANGUAGES
    CXX C
)

file(GLOB lore_SOURCES CONFIGURE_DEPENDS "include/lore/**/*.h" "src/*.cpp" "src/**/*.cpp")

add_library(lore ${lore_SOURCES})
target_include_directories(lore PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_compile_features(lore PUBLIC cxx_std_14)

if(${ENABLE_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()
