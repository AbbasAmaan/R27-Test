cmake_minimum_required(VERSION 3.20)

project(r27_test)

add_library(
  r27 
  src/queue.c
  src/mutex.c 
  src/en_dc.c
  src/file.c
  src/drive.c
)

target_include_directories(
  r27 
  PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/lib 
  
) 

add_executable(
  queue_test 
  src/main.c 
)
target_link_libraries(
  queue_test 
  PRIVATE 
  r27
  m
)
