cmake_minimum_required(VERSION 2.6)
project(todo_backend_cppcms)


find_library(CPPCMS cppcms)
find_library(BOOSTER booster)
find_library(CPPDB cppdb)


find_path(CPPCMS_INC cppcms/application.h)
find_path(BOOSTER_INC booster/shared_ptr.h)
find_path(CPPDB_INC cppdb/frontend.h)


include_directories(${CPPCMS_INC})
include_directories(${BOOSTER_INC})
include_directories(${CPPDB_INC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp_src)

add_definitions("-std=c++11")

add_executable(todo_backend_cppcms cpp_src/todoitem.cpp cpp_src/todos.cpp cpp_src/main.cpp)

target_link_libraries(todo_backend_cppcms ${CPPCMS} ${CPPDB} ${BOOSTER})
# copy all contents of sub-directory 'config' into 'build' dir
add_custom_command(TARGET todo_backend_cppcms PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/config/ $<TARGET_FILE_DIR:todo_backend_cppcms>)
                   

install(TARGETS todo_backend_cppcms RUNTIME DESTINATION bin)

#add_subdirectory(config)
# create empty dir for logging in build directory:
add_subdirectory(log/lighttpd)