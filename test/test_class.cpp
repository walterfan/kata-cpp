#include "test_class.h"

using namespace std;

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