#include "run_example.h"

using namespace std;
namespace po = boost::program_options;

extern int std_function_test(int argc, char** argv);
extern int function_demo(int argc, char** argv);
extern int lambda_demo(int argc, char* argv[]);
extern int rvalue_demo(int argc, char* argv[]);
extern int smart_ptr_demo(int argc, char** argv);

extern int asio_timer_demo_1(int argc, char** argv);
extern int asio_timer_demo_2(int argc, char** argv);

extern int asio_udp_demo(int argc, char** argv);

const char* usage = R"name(please specify example name:
    function_demo
    or lambda_demo
    or rvalue_demo
    or smart_ptr_demo
    or std_function_test
    or ...
)name";



//----------------------- ExampleRunner ------------------------------//
ExampleRunner::ExampleRunner(): m_example_count(0),m_examples() {
    BOOST_LOG_TRIVIAL(trace)<<"* ExampleRunner construct: " ;
}

ExampleRunner::~ExampleRunner() {
    BOOST_LOG_TRIVIAL(trace)<<"* ExampleRunner destruct: ";
}

void ExampleRunner::init() {
    register_example("function_demo", function_demo);
    register_example("smart_ptr_demo", smart_ptr_demo);
    register_example("lambda_demo", lambda_demo);
	register_example("rvalue_demo", rvalue_demo);
    register_example("std_function_test", std_function_test);
    register_example("asio_time_demo_1", asio_timer_demo_1);
    register_example("asio_time_demo_2", asio_timer_demo_2);

}

void ExampleRunner::register_example(const string& name, const exam_func_t &exam)
{
    m_example_count++;
    m_examples[name] = exam;
    m_commands[m_example_count] = name;
}

int ExampleRunner::execute_example(const string& name, int argc, char** argv) const
{
    auto it = m_examples.find(name);
    if(it != m_examples.end()) {
        BOOST_LOG_TRIVIAL(trace) << "execute "<< it->first;
        exam_func_t func = it->second;
        return func(argc, argv);
    }
    BOOST_LOG_TRIVIAL(trace) << "not registered "<< name;
    return -1;
}

exam_func_ptr ExampleRunner::find_example(int exampleId) const {
    auto it = m_commands.find(exampleId);
    if(it != m_commands.end()) {
        return find_example(it->second);
    }
    return nullptr;
}

exam_func_ptr ExampleRunner::find_example(const std::string& exampleName) const {

    auto it = m_examples.find(exampleName);
    if(it != m_examples.end()) {
        return it->second;
    }
    return nullptr;
}


size_t ExampleRunner::size() const {
    return m_examples.size();
}

void ExampleRunner::display_menu() const {
    cout<<"===================================================== \n";
    cout<<" \t\tMENU \t \n ";
    cout<<"===================================================== \n";


    for(auto&& [first,second]: m_commands) {
        cout << " " << first << ". " << second << endl;
    }
    cout<<" 0. quit\n";

}

int main(int argc, char** argv)
{
    unique_ptr<ExampleRunner> runner = my_make_unique<ExampleRunner>();
    runner->init();

    //c++11 R"raw string"
    po::options_description desc("Allowed options:");

    desc.add_options()
        ("help,h", "produce help message")
        ("name,n", po::value<string>(), usage)
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        BOOST_LOG_TRIVIAL(trace) << desc << "\n";
        return 1;
    }

    boost::log::core::get()->set_filter (
        boost::log::trivial::severity >= boost::log::trivial::info
    );

    if (vm.count("name")) {
        BOOST_LOG_TRIVIAL(trace) << "* example name is "<< vm["name"].as<string>() << ".";
        runner->execute_example(vm["name"].as<string>(), argc, argv);
    } else {
        runner->display_menu();
        int nChoice = 0;
        do {
            cout<<"Enter your choice(0-" << runner->size() << "):";
            cin>>nChoice;

            auto func_ptr = runner->find_example(nChoice);
            if(func_ptr) {
                func_ptr(argc, argv);
            } else {
                cout<<"Invalid option, please select again"<<endl; runner->display_menu();
            }

        } while (nChoice > 0);
    }

    return 0;
}
