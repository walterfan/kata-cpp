include_directories(${CMAKE_SOURCE_DIR}/inc)
link_directories(/usr/lib/x86_64-linux-gnu)

set (CMAKE_CXX_STANDARD 17)
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)

add_compile_definitions(BOOST_LOG_DYN_LIN)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

FIND_PACKAGE(Boost 1.74.0 REQUIRED COMPONENTS system thread regex log program_options)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIR}")

set(SRCS run_example.cpp
    boost_function.cpp
    boost_memory.cpp
    boost_asio_timer.cpp
    boost_asio_chat.cpp
    std_function_exam.cpp
    std_lambda.cpp
    std_thread.cpp
    tiny_command.cpp
    AudioLevelCalculator.cpp
)

ADD_EXECUTABLE(run_example ${SRCS})
TARGET_LINK_LIBRARIES( run_example LINK_PUBLIC ${Boost_LIBRARIES} )




