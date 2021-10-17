cmake_minimum_required(VERSION 3.15)

project(uot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FETCHCONTENT_QUIET OFF)
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY "https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent"
  GIT_TAG        "v3.9.1"
)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
FetchContent_MakeAvailable(json)

add_subdirectory(uot)
add_subdirectory(uot-server)
add_subdirectory(libuot)
add_subdirectory(uot-client)
