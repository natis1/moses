cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

set(PROJECT_NAME moses)


file(GLOB SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/include/*.h")


set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")


include_directories(${CMAKE_SOURCE_DIR}/include)

message("${SOURCE_FILES} are files used")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#target_link_libraries(${PROJECT_NAME} ${LIB_FILES})
