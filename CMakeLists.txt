cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

set(PROJECT_NAME moses)


file(GLOB SOURCE_FILES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/include/*.h")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")


set(${PROJECT_NAME}_SEACAS_PATH "/home/eli/seacaslibs/seacas" CACHE FILEPATH "Specify where you installed seacas.")
set(${PROJECT_NAME}_MPI_PATH "/home/eli/seacaslibs/openmpi-1.6.4" CACHE FILEPATH "Specify where you installed MPI.")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(SYSTEM ${${PROJECT_NAME}_MPI_PATH}/include)
include_directories(SYSTEM ${${PROJECT_NAME}_SEACAS_PATH}/include)

set(CMAKE_CXX_COMPILER ${${PROJECT_NAME}_MPI_PATH}/bin/mpicxx)
set(CMAKE_C_COMPILER ${${PROJECT_NAME}_MPI_PATH}/bin/mpicc)
set(CMAKE_Fortran_COMPILER ${${PROJECT_NAME}_MPI_PATH}/bin/mpif77)

set(${PROJECT_NAME}_MPI_LIB "-L${${PROJECT_NAME}_MPI_PATH}/lib -lmpi")
set(${PROJECT_NAME}_SEACAS_LIB "-L${${PROJECT_NAME}_SEACAS_PATH}/lib -lexoIIv2c -lnetcdf -lhdf5_hl -lhdf5 -lchaco")


#${${PROJECT_NAME}_MPI_LIB} 
set(${PROJECT_NAME}_LIBS "${${PROJECT_NAME}_MPI_LIB} ${${PROJECT_NAME}_SEACAS_LIB}")

message("${SOURCE_FILES} are files used")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})

#target_link_libraries(${PROJECT_NAME} ${LIB_FILES})
