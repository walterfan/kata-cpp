import os

LIB_PATH = ['/usr/lib', '/usr/local/lib', '/usr/local/opt/openssl/lib']

env = Environment()
env.Append(CPPPATH=["/usr/local/include", "/usr/include/jsoncpp"] )
env.Append(CCFLAGS = '-fno-elide-constructors -Wall -g -O1 -static -std=c++20 -DBOOST_LOG_DYN_LINK')
env.Append(LIBPATH=["/usr/local/lib"])

tinydiagram=env.Program(target='./bin/tiny_diagram', source=['./exam/tiny_diagram.cpp'])

wordbankdemo=env.Program(target='./bin/wordbankdemo',
    source=['./exam/WordBankDemo.cpp', './exam/WordBank.cpp'],
	CCFLAGS='-fno-elide-constructors -Wall -g -O1 -static -std=c++17 -DBOOST_LOG_DYN_LINK',
    #LINKFLAGS='-pg',
    LIBS = ['boost_log','boost_timer'],
    LIBPATH = LIB_PATH )

run_example=env.Program(target='./bin/run_example', source=[
    './exam/run_example.cpp',
    './exam/boost_function.cpp',
    './exam/boost_memory.cpp',
    './exam/boost_asio_timer.cpp',
    './exam/boost_asio_strand.cpp',
    './exam/std_function_exam.cpp',
    './exam/crtp_demo.cpp',
    './exam/std_lambda.cpp',
    './exam/std_thread.cpp',
    './exam/std_visit_demo.cpp',
    './exam/tiny_command.cpp',
    './exam/AudioLevelCalculator.cpp'],
    LIBS = ['pthread','boost_log','boost_thread', 'boost_program_options'],
    LIBPATH = LIB_PATH)


unittest=env.Program(target='./bin/unit_test', source=[
    './test/unit_test.cpp',
    './test/quick_test.cpp',
    './test/invoke_test.cpp',
    './test/ntptime_test.cpp',
    './test/time_point_test.cpp',
    './test/chrono_duration_test.cpp',
    './test/smart_ptr_test.cpp',
	'./test/emcpp_7.cpp'],
    LIBS = ['gmock','gtest', 'pthread'], LIBPATH = LIB_PATH)

Default(run_example,
    wordbankdemo,
    unittest)

