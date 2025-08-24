#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace testing;

static int s_instance_count = 0;

class Grandpa {
public:
    Grandpa() {
        cout << "Grandpa" << endl;
        ++s_instance_count;
    }
    virtual ~Grandpa() {
        cout << "~Grandpa" << endl;
        --s_instance_count;
    }
};

class Father: public Grandpa {
public:
    Father() {
        cout << "Father" << endl;
        ++s_instance_count;
    }
    ~Father() {
        cout << "~Father" << endl;
        --s_instance_count;
    }
};

class Child: public Father {
public:
    Child() {
        cout << "Child" << endl;
        ++s_instance_count;
    }
    ~Child()  {
        cout << "~Child" << endl;
        --s_instance_count;
    }
};


TEST(OverrideTest, testDeconstruct)
{
    Father* pFather = new Child();

    delete pFather;
    ASSERT_EQ(s_instance_count, 0);
}