# Enabling debug symbols
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

list (APPEND CMAKE_CXX_FLAGS "-w -g -std=c++11${CMAKE_CXX_FLAGS}")

file(GLOB SRCS *.cpp *.h)

add_executable(SERVER ${SRCS})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
message(CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH})
find_package(SFML 2.2.0 REQUIRED system window graphics network audio)

if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(SERVER ${SFML_LIBRARIES})
endif()
