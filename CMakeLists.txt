cmake_minimum_required(VERSION 3.1)

project(potato)

add_library(potato SHARED dvpl.c)

target_link_libraries(potato "lz4")

set_target_properties(potato PROPERTIES SOVERSION 1)
set_target_properties(potato PROPERTIES VERSION 1.0.1)

set_target_properties(potato PROPERTIES PUBLIC_HEADER "dvpl.h;potato.h")

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()
if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()
install(TARGETS potato LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#add_executable(${PROJECT_NAME} "main.c")
