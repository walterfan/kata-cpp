include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(/usr/include/x86_64-linux-gnu)
link_directories(/usr/lib/x86_64-linux-gnu)

# pls run apt install -y libboost1.74-all-dev or above
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)

#SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
#SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")


#SET(PY_COMPILE_FLAGS "-I/Users/yafan/opt/anaconda3/include/python3.9 -I/Users/yafan/opt/anaconda3/include/python3.9")
#SET(PY_LINK_FLAGS "-ldl -framework CoreFoundation")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${PY_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} ${PY_LINK_FLAGS}")

add_compile_definitions(BOOST_LOG_DYN_LIN)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

FIND_PACKAGE(Boost 1.71.0 REQUIRED COMPONENTS thread system regex log program_options)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIR}")


list (APPEND EXAM_SOURCES
    run_example.cpp
    boost_function.cpp
    boost_asio_timer.cpp
    boost_asio_strand.cpp
    boost_thread_pool.cpp
    boost_lockfree_queue.cpp
    boost_queue_demo.cpp
    std_function_exam.cpp
    crtp_demo.cpp
    std_lambda.cpp
    std_thread.cpp
    enable_shared_demo.cpp
    std_visit_demo.cpp
    tiny_command.cpp
    AudioLevelCalculator.cpp
)

ADD_EXECUTABLE(run_example ${EXAM_SOURCES})
TARGET_LINK_LIBRARIES( run_example LINK_PUBLIC ${Boost_LIBRARIES} )




