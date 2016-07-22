cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

set(PROJECT_NAME moses)


file(GLOB SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/include/*.h")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")


set(${PROJECT_NAME}_SEACAS_PATH "${CMAKE_SOURCE_DIR}/dep/seacas" CACHE FILEPATH "Specify where you installed seacas.")


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${${PROJECT_NAME}_SEACAS_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/dep/dlib)

set(${PROJECT_NAME}_SEACAS_LIB "-L${${PROJECT_NAME}_SEACAS_PATH}/lib -lexoIIv2c -lnetcdf -lhdf5_hl -lhdf5 -ldlib -pthread")
set(${PROJECT_NAME}_LIBS "${${PROJECT_NAME}_SEACAS_LIB}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})