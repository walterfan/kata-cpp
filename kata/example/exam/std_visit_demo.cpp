#include <variant>
#include <iostream>

struct A {
    std::string name = "spencer";
};

struct B {
    std::string type = "person";
};

struct C {
    double age = 5;
};


template<typename...Func>
struct overload : Func... {
    using Func::operator()...;
};

template<typename...Func> overload(Func...) -> overload<Func...>;


int std_visit_demo(int argc, char** argv)
{
    overload ovld {
        [](A a) { std::cout << a.name << std::endl; },
        [](B b) { std::cout << b.type << std::endl; },
        [](C c) { std::cout << c.age << std::endl; }
    };

    std::variant<A, B, C> something{B{}};

    std::visit(ovld, something);
    return 0;
}