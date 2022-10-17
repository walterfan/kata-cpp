#ifndef __TEST_CLASS_H__
#define __TEST_CLASS_H__


#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <optional>


struct Student {
    std::string m_id;
    std::string m_name;
    std::optional<uint8_t> m_age;

    Student(const std::string& id, const std::string& name):m_id(id), m_name(name) {}
    Student(const std::string& id, const std::string& name, uint8_t age):m_id(id), m_name(name),m_age(age) {}

    const std::string& get_id() { return m_id; }
    const std::string& get_name() { return m_name; }
    uint8_t get_age() { return m_age.has_value()? m_age.value(): 0; }


};

inline bool operator==(const Student& lhs, const Student& rhs) {
    return ((lhs.m_id == rhs.m_id) && (lhs.m_name == rhs.m_name));
}

inline bool operator!=(const Student& lhs, const Student& rhs) {
    return !(lhs == rhs);
}


class School {
public:
    void add_student(const Student& s);
    std::optional<Student> get_student(const std::string& id);
    std::vector<Student> m_students;
};



#endif