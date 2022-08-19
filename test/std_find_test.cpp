#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>


using namespace std;
using namespace testing;

struct Student {
    string m_id;
    string m_name;
    Student(const string& id, const string& name):m_id(id), m_name(name) {}
    const string& get_id() { return m_id; }
};

class School {
public:
    void add_student(const Student& s);
    optional<Student> get_student(const std::string& id);
    vector<Student> m_students;
};

void School::add_student(const Student& s) {
    m_students.push_back(s);
}

optional<Student> School::get_student(const std::string& id)
{
    auto is_same_id = [id](Student s){ return s.get_id() == id; };
    auto ret = std::find_if(m_students.begin(), m_students.end(), is_same_id);
    if(ret != m_students.end()) {
        return optional<Student>(*ret);
    }
    return optional<Student>();

}

TEST(FindIfTest, testcase1)
{
    School school;
    school.add_student({"1", "alice"});
    school.add_student({"2", "bob"});
    school.add_student({"3", "carl"});

    optional<Student> ret = school.get_student("2");
    cout << ret->m_name << endl;
    ASSERT_EQ(ret->m_name, "bob");
}