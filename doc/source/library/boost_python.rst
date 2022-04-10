Boost Python
======================

.. code-block:: c++

    char const* greet()
    {
        return "hello, world";
    }

    #include <boost/python.hpp>
    
    BOOST_PYTHON_MODULE(hello_ext)
    {
        using namespace boost::python;
        def("greet", greet);
    }

* Makefile:

.. code-block::

    # location of the Python header files
    PYTHON_VERSION = 2.7
    PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)
    # location of the Boost Python include files and library
    BOOST_INC = /usr/include
    BOOST_LIB = /usr/lib
    # compile mesh classes
    TARGET = hello_ext
    $(TARGET).so: $(TARGET).o
        g++ -shared -Wl,--export-dynamic $(TARGET).o -L$(BOOST_LIB) -lboost_python-$(PYTHON_VERSION) -L/usr/lib/python$(PYTHON_VERSION)/config -lpython$(PYTHON_VERSION) -o $(TARGET).so
    $(TARGET).o: $(TARGET).C
        g++ -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC -c $(TARGET).C


* execute

.. code-block::

    import hello_ext
    print(hello_ext.greet())
        