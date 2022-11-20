include_directories(${CMAKE_SOURCE_DIR}/inc)
link_directories(/usr/lib/x86_64-linux-gnu)

set (CMAKE_CXX_STANDARD 17)
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")


SET(PY_COMPILE_FLAGS "-I/Users/yafan/opt/anaconda3/include/python3.9 -I/Users/yafan/opt/anaconda3/include/python3.9")
SET(PY_LINK_FLAGS "-ldl -framework CoreFoundation")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${PY_COMPILE_FLAGS}")
 SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} ${PY_LINK_FLAGS}")

add_compile_definitions(BOOST_LOG_DYN_LIN)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

FIND_PACKAGE(Boost 1.71.0 REQUIRED COMPONENTS system regex log program_options)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

message("boost lib: ${Boost_LIBRARIES}")
message("boost inc:${Boost_INCLUDE_DIR}")

file(GLOB exam_SRC
     "*.h"
     "*.cpp"
)

ADD_EXECUTABLE(run_example ${exam_SRC})
TARGET_LINK_LIBRARIES( run_example LINK_PUBLIC ${Boost_LIBRARIES} )




