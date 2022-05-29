#pragma once


#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <functional>

using command_executor = std::function<int(int, char**)>;

class Command
{
public:
    Command();
    Command(std::string name);

    Command(const Command& other);
    Command& operator=(const Command& other);

    Command(Command&& other);
    Command& operator=(Command&& other);

    virtual ~Command();

    void setName(const std::string& name);
    void setParameter(const std::string& name, const std::string& value);
    void setData(const uint8_t* pData, size_t length);

    friend std::ostream& operator<<(std::ostream&, const Command& obj);

private:
    std::string m_name = "";
    std::map<std::string, std::string> m_parameters;
    size_t m_length = 0;
    uint8_t* m_data = nullptr;
    command_executor m_executor = nullptr;
};
