cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

set(PROJECT_NAME moses)


file(GLOB SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/include/*.h")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

#set(CMAKE_LINKER "g++")
#set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} ${FLAGS} ${CMAKE_CXX_LINK_FLAGS} ${LINK_FLAGS} ${OBJECTS} -o ${TARGET} ${LINK_LIBRARIES}")


include_directories(${CMAKE_SOURCE_DIR}/include)


target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_SEACAS_LIB})

message("${SOURCE_FILES} are files used")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#target_link_libraries(${PROJECT_NAME} ${LIB_FILES})
