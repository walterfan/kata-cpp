import os

LIB_PATH = ['/usr/lib', '/usr/local/lib', '/usr/local/opt/openssl/lib']

env = Environment()
env.Append(CPPPATH=["/usr/local/include", "/usr/include/jsoncpp", "./exan"] )
env.Append(CCFLAGS = '-fno-elide-constructors -Wall -g -O1 -static -std=c++20 -DBOOST_LOG_DYN_LINK')
env.Append(LIBPATH=["/usr/local/lib"])

tinydiagram=env.Program(target='./bin/tiny_diagram', source=['./exam/tiny_diagram.cpp'])

wordbankdemo=env.Program(target='./bin/wordbankdemo',
    source=['./exam/WordBankDemo.cpp', './exam/WordBank.cpp'],
	CCFLAGS='-fno-elide-constructors -Wall -g -O1 -static -std=c++20 -DBOOST_LOG_DYN_LINK',
    #LINKFLAGS='-pg',
    LIBS = ['boost_log','boost_timer'],
    LIBPATH = LIB_PATH )

wordbankdemo=env.Program(target='./bin/state_pattern',
    source=['./pattern/state_pattern.cpp'],
	CCFLAGS='-fno-elide-constructors -Wall -g -O1 -static -std=c++20 -DBOOST_LOG_DYN_LINK',
    #LINKFLAGS='-pg',
    LIBS = ['boost_log','boost_timer'],
    LIBPATH = LIB_PATH )

run_example=env.Program(target='./bin/run_example', source=[
    './exam/run_example.cpp',
    './exam/boost_function.cpp',
    './exam/boost_asio_timer.cpp',
    './exam/boost_asio_strand.cpp',
    './exam/boost_thread_pool.cpp',
    './exam/boost_lockfree_queue.cpp',
    './exam/boost_queue_demo.cpp',
    './exam/std_function_exam.cpp',
    './exam/crtp_demo.cpp',
    './exam/std_lambda.cpp',
    './exam/std_thread.cpp',
    './exam/enable_shared_demo.cpp',
    './exam/std_visit_demo.cpp',
    './exam/tiny_command.cpp',
    './exam/AudioLevelCalculator.cpp'],
    LIBS = ['pthread','boost_log','boost_thread', 'boost_program_options'],
    CPPPATH = ["./exam"],
    LIBPATH = LIB_PATH)


unittest=env.Program(target='./bin/unit_test', source=[
    './test/unit_test.cpp',
    './test/test_class.cpp',
    './test/template_test.cpp',
    './test/quick_test.cpp',
    './test/invoke_test.cpp',
    './test/ntptime_test.cpp',
    './test/time_point_test.cpp',
    './test/chrono_duration_test.cpp',
    './test/smart_ptr_test.cpp',
    './test/std_set_test.cpp',
    './test/std_map_test.cpp',
    './test/std_find_test.cpp',
    './test/std_optional_test.cpp',
    './test/override_test.cpp',
    './test/struct_test.cpp',
    './test/waltertest.cpp',
	'./test/emcpp_7.cpp'],
    LIBS = ['gmock','gtest', 'pthread'],CPPPATH = ["./exam"], LIBPATH = LIB_PATH)


overload_pattern_demo=env.Program(target='./bin/overload_pattern_demo',
    source=['./pattern/overload_pattern.cpp'])

linear_regression_demo=env.Program(target='./bin/linear_regression_demo',
    source=['./algorithm/linear_regression.cpp'])
tema_demo=env.Program(target='./bin/tema_demo',
    source=['./algorithm/tema.cpp'])
ema_demo=env.Program(target='./bin/ema_demo',
    source=['./algorithm/ema.cpp'])
tma_demo=env.Program(target='./bin/tma_demo',
    source=['./algorithm/tma.cpp'])

leaky_bucket_demo=env.Program(target='./bin/leaky_bucket_demo',
    source=['./algorithm/leaky_bucket.cpp'])
token_bucket_demo=env.Program(target='./bin/token_bucket_demo',
    source=['./algorithm/token_bucket.cpp'])


Default(run_example,
        linear_regression_demo,
        leaky_bucket_demo,
        token_bucket_demo,
        tma_demo,
        ema_demo,
        tema_demo,
        overload_pattern_demo,
        wordbankdemo,
        unittest)

