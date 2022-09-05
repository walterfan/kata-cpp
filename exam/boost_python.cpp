#include <boost/python.hpp>
/*

python3.9-config --includes --libs
-I~/opt/anaconda3/include/python3.9 -I~/opt/anaconda3/include/python3.9
-ldl -framework CoreFoundation
*/
char const* greet()
{
   return "hello, world";
}

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    def("greet", greet);
}