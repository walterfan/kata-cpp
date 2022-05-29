#include "tiny_command.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace std;

//C++11: delegate constructor
Command::Command():Command("") {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;
}

Command::Command(string name):m_name(name) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;    
}

Command::Command(const Command& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". copy construct: " << m_name << "@" <<this;
    m_name = other.m_name;
    for (const auto& kv : other.m_parameters) {
        BOOST_LOG_TRIVIAL(trace) << kv.first << " has value " << kv.second;
        m_parameters.insert(kv);
    }
    std::copy(other.m_data, other.m_data + m_length, m_data);
}

Command& Command::operator=(const Command& other) {
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". operator =: " << other.m_name;
    if (this != &other)
    {
        this->m_name = other.m_name;
        for (const auto& kv : other.m_parameters) {
            std::cout << kv.first << " has value " << kv.second << std::endl;
            m_parameters.insert(kv);
        }
        // Free the existing resource.
        if(m_data) {
            delete[] m_data;
        }
        m_length = other.m_length;
        m_data = new uint8_t[m_length];
        std::copy(other.m_data, other.m_data + m_length, m_data);
    }
    return *this;
}

Command::Command(Command&& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move copy construct: " << other.m_name << "@" <<this;
    m_name = std::move(other.m_name);
    m_parameters = std::move(other.m_parameters);

    // Copy the data pointer and its length from the source object.
    m_data = other.m_data;
    m_length = other.m_length;

    // Release the data pointer from the source object so that
    // the destructor does not free the memory multiple times.
    other.m_data = nullptr;
    other.m_length = 0;

}

Command& Command::operator=(Command&& other)
{
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move operator ==" << other.m_name;
   if (this != &other)
   {
      m_name = std::move(other.m_name);
      m_parameters = std::move(other.m_parameters);
      // Free the existing resource.
      delete[] m_data;

      // Copy the data pointer and its length from the
      // source object.
      m_data = other.m_data;
      m_length = other.m_length;

      // Release the data pointer from the source object so that
      // the destructor does not free the memory multiple times.
      other.m_data = nullptr;
      other.m_length = 0;
   }
   return *this;
}

Command::~Command() {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". destruct: " << m_name ;
}


void Command::setName(const string& name) {
    m_name = name;
}
void Command::setParameter(const string& name, const string& value) {
    m_parameters[name] = value;
}

void Command::setData(const uint8_t* pData, size_t length) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". setData: " << m_name;
	if(nullptr != m_data) {
        delete[] m_data;
    }

    m_length = length;
	m_data = new uint8_t[length];
    std::copy(pData, pData + length, m_data);
}

ostream& operator<<(ostream& os, const Command& obj)
{
    os << obj.m_name;
    os << ": ";
    //for (map<string, string>::iterator it = obj.m_parameters.begin(); it != obj.m_parameters.end(); ++it)
    for (const auto& kv : obj.m_parameters)
    {
        os << kv.first << "=" << kv.second << endl;
    }
    for(size_t i=0; i< obj.m_length; ++i) {
        os << *(obj.m_data + i);
    }
    return os;
}
