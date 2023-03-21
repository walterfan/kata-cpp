#ifndef RUN_EXAMPLE_H_
#define RUN_EXAMPLE_H_

#include <stdio.h>
#include <stdint.h>

#include <string>
#include <map>
#include <iostream>
#include <memory>

#include <unordered_map>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <boost/program_options.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/assert.hpp>

#ifndef DEBUG_TRACE
#define DEBUG_TRACE(msg) do { \
            std::cout <<"["<<time(NULL)<<","<< __FILE__ << "," << __LINE__ << "]\t"<< msg << std::endl; \
        } while(0)
#endif

//old function pointer
//typedef int (*exam_func_ptr)(int argc, char** argv);
//new function object
typedef std::function<int(int, char**)> exam_func_t;

using exam_func_ptr = std::function<int(int, char**)>;

class ExampleRunner: boost::noncopyable {
public:
    ExampleRunner();
    //no need by noncopyable
    //ExampleRunner(const ExampleRunner& rhs);
    //ExampleRunner& operator=(const ExampleRunner& rhs);

    virtual ~ExampleRunner();

    void init();

    size_t size() const;

    void register_example(const std::string& name, const exam_func_t &exam);

    int execute_example(const std::string& name, int argc, char** argv) const;

    exam_func_ptr find_example(int exampleId) const;

    exam_func_ptr find_example(const std::string&exampleId) const;

    void display_menu() const;
private:
    uint32_t m_example_count = 0;
    std::unordered_map<std::string, exam_func_t> m_examples;
    std::map<int, std::string> m_commands;
};


#endif