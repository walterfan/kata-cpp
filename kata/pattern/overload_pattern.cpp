#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

template<typename ... Args>
void printMe(Args&& ... args) {
    (std::cout <<  ... <<  std::forward<Args>(args)) << '\n';
}

// the variant to visit
using var_t = std::variant<int, long, double, std::string>;
 
// helper constant for the visitor #3
template<class> inline constexpr bool always_false_v = false;
 
// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class PrintVisitor {
public:
    void operator()(int i) {
        std::cout << "(int)" << i << std::endl;
    }
    void operator()(long i) {
        std::cout << "(long)" << i << std::endl;
    }
    void operator()(double i) {
        std::cout << "(double)" << i << std::endl;
    }
    void operator()(std::string str) {
        std::cout << "(string)" << str << std::endl;
    }
};

int main() {


    std::vector<var_t> vec = {10, 15l, 1.5, "hello"};

    printMe(
        std::get<int>(vec[0]), ", ",
        std::get<long>(vec[1]), ", ",
        std::get<double>(vec[2]),", ",
        std::get<std::string>(vec[3]));

    for (auto& v: vec) {

        // 1. void visitor, only called for side-effects (here, for I/O)
        std::visit([](auto&& arg){std::cout << arg;}, v);
 
        // 2. value-returning visitor, demonstrates the idiom of returning another variant
        var_t w = std::visit([](auto&& arg) -> var_t {return arg + arg;}, v);
 
        // 3. type-matching visitor: a lambda that handles each type differently
        std::cout << ". After doubling, variant holds ";
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
                std::cout << "int with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, long>)
                std::cout << "long with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, double>)
                std::cout << "double with value " << arg << '\n';
            else if constexpr (std::is_same_v<T, std::string>)
                std::cout << "std::string with value " << std::quoted(arg) << '\n';
            else 
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
        }, w);
    }


    std::cout << "-------- visit variant with PrintVisitor --------" << std::endl;
    for (auto& v: vec) {
        std::visit(PrintVisitor(), v);
    }

    std::cout << "-------- visit variant with overloaded operator --------" << std::endl;
    for (auto& v: vec) {
        // 4. another type-matching visitor: a class with 3 overloaded operator()'s
        // Note: The `(auto arg)` template operator() will bind to `int` and `long`
        //       in this case, but in its absence the `(double arg)` operator()
        //       *will also* bind to `int` and `long` because both are implicitly
        //       convertible to double. When using this form, care has to be taken
        //       that implicit conversions are handled correctly.
        std::visit(overloaded {
            [](auto arg) { std::cout << arg << std::endl; },
            [](double arg) { std::cout << std::fixed << arg << std::endl; },
            [](const std::string& arg) { std::cout << std::quoted(arg) << std::endl; }
        }, v);
    }
}
