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
    './exam/std_function_exam.cpp',
    './exam/cpp11_lambda.cpp',
    './exam/AudioLevelCalculator.cpp'],
    LIBS = ['jsoncpp', 'pthread','boost_log','boost_thread', 'boost_program_options'],
    LIBPATH = LIB_PATH)

Default(run_example,
    wordbankdemo)

