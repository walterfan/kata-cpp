#include "tiny_command.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

using namespace std;


int rvalue_demo(int argc, char* argv[])
{
    //move constructor
    Command cmd1(Command("c1"));
    //copy constructor
    Command cmd2 = cmd1;

    shared_ptr<Command> sharedPtr = make_shared<Command>("update");
    sharedPtr->setParameter("user", "alice");
    uint8_t nBytes[3]   = { 0x00,0x01,0x02 };
    sharedPtr->setData(nBytes, 3);
    
    shared_ptr<Command> sharedPtr2 = sharedPtr;
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". sharedPtr=" << sharedPtr.get() << ", "<< sharedPtr.use_count() <<", command="<< *sharedPtr;
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". sharedPtr2=" << sharedPtr2.get() << ", "<< sharedPtr2.use_count() <<", command="<< *sharedPtr2;

    return 0;
}

int smart_ptr_demo(int argc, char* argv[])
{

    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << " -- unique_ptr --";
    //unique_ptr<Command> ptr = unique_ptr<Command>(new Command());
    unique_ptr<Command> uniquePtr = make_unique<Command>();
    uniquePtr->setName("create");
    uniquePtr->setParameter("user", "walter");
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". unique pointer=" << uniquePtr.get() <<", command="<< *uniquePtr;

    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". -- shared_ptr --";

    shared_ptr<Command> sharedPtr = make_shared<Command>();
    sharedPtr->setName("retrieve");
    sharedPtr->setParameter("user", "walter");
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". shared pointer=" << sharedPtr.get() << ", use_count="<< sharedPtr.use_count() <<", command="<< *sharedPtr;

    shared_ptr<Command> sharedPtr2 = sharedPtr;
    assert(sharedPtr == sharedPtr2 && sharedPtr.use_count() == 2);

    sharedPtr.reset();
    assert(!sharedPtr);

    return 0;
}


class Entity: public enable_shared_from_this<Entity> {
public:
    //correct
    shared_ptr<Entity> getThisEntity() {
        return shared_from_this();
    }
    //dangerous of double delete
    shared_ptr<Entity> getThisSharedPtr() {
        return shared_ptr<Entity>(this);
    }

    //dangerous of the raw pointer
    Entity* getThisPtr() {
        return this;
    }

    const std::string& getVersion() const {
        return this->version;
    }
private:
    std::string version = "1.0";
};

int enable_shared_from_this_demo(int argc, char* argv[]) {
    auto ptr1 = make_shared<Entity>();
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". version=" << ptr1->getVersion() ;

    auto ptr2 = ptr1->getThisEntity();
    BOOST_LOG_TRIVIAL(info) << "line " <<__LINE__ << ". version=" << ptr2->getVersion() ;

    return 0;
}