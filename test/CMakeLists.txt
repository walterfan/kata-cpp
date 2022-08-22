set(CXX_FLAGS
 -g
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Wold-style-cast
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 -std=c++2a
 )

add_executable(unit_test
    unit_test.cpp
    quick_test.cpp
    #invoke_test.cpp
    #ntptime_test.cpp
    time_point_test.cpp
    chrono_duration_test.cpp
    smart_ptr_test.cpp
    std_set_test.cpp
    #std_find_test.cpp
    override_test.cpp
	emcpp_7.cpp
)
target_link_libraries(unit_test gtest pthread)

