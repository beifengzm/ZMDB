cmake_minimum_required(VERSION 3.19)

project(ZMDB)

add_definitions("-g -Wall")

include_directories(include/db)
include_directories(include/net)
include_directories(include/base)
include_directories(include/command)

aux_source_directory(src/net NET_SRCS)
add_library(netlib ${NET_SRCS})
aux_source_directory(src/db DB_SRCS)
add_library(dblib ${DB_SRCS})
aux_source_directory(src/base BASE_SRCS)
add_library(baselib ${BASE_SRCS})
aux_source_directory(src/command COMMAND_SRCS)
add_library(commandlib ${COMMAND_SRCS})

add_executable(HashTest test/HashTest.c)
target_link_libraries(HashTest baselib)

add_executable(ServerTest test/ServerTest.c)
target_link_libraries(ServerTest netlib commandlib dblib baselib)

add_executable(ClientTest test/ClientTest.c)
target_link_libraries(ClientTest netlib commandlib baselib)

add_executable(ConfigTest test/ConfigTest.c)
target_link_libraries(ConfigTest baselib)

add_executable(DebugTest test/DebugTest.c)
target_link_libraries(DebugTest baselib)

add_executable(DatabaseTest test/DatabaseTest.c)
target_link_libraries(DatabaseTest dblib baselib)

add_executable(TimerTest test/TimerTest.c)
target_link_libraries(TimerTest dblib baselib)

add_executable(Priority_queueTest test/Priority_queueTest.c)
target_link_libraries(Priority_queueTest dblib baselib)

add_executable(PersistTest test/PersistTest.c)
target_link_libraries(PersistTest netlib commandlib dblib baselib)

add_executable(SetTest test/SetTest.c)
target_link_libraries(SetTest netlib dblib baselib commandlib)