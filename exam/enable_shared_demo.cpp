#include "run_example.h"
#include <string>
#include <map>
#include <memory>

using namespace std;

class Animal;

class Zookeeper {
public:
    void addAnimal(std::shared_ptr<Animal> animal);
    uint32_t getAnimalCount(const std::string& name);
    const std::shared_ptr<Animal>& getAnimal(const std::string& name);
private:
    std::map<std::string, std::shared_ptr<Animal>> m_animals;
    std::map<std::string, uint32_t> m_counter;
    std::shared_ptr<Animal> m_empty_animal = nullptr;
};

class Animal: public std::enable_shared_from_this<Animal> {
public:

    Animal(const std::string name, const std::shared_ptr<Zookeeper>& keeper)
    :m_name(name), m_keeper(keeper) {
        //!!! dangerous, the construtor have not been done, will crash if
        //m_keeper->addAnimal(getThisEntity());
    }
    //correct
    std::shared_ptr<Animal> getThisEntity() {
        return shared_from_this();
    }
    //dangerous of double delete
    std::shared_ptr<Animal> getThisSharedPtr() {
        return shared_ptr<Animal>(this);
    }

    //dangerous of the raw pointer
    Animal* getThisPtr() {
        return this;
    }

    const std::string& getName() const {
        return this->m_name;
    }

    void snarl() {
        if (m_name == "dog") {
            std::cout << "wangwang!" << std::endl;
        } else {
            std::cout << "haha!" << std::endl;
        }

    }
private:
    std::string m_name;
    std::shared_ptr<Zookeeper> m_keeper;
};


void Zookeeper::addAnimal(std::shared_ptr<Animal> animal) {
    if(auto[iter, inserted]{ m_counter.insert({animal->getName(), 1}) }; !inserted) {
        iter->second ++;
        DEBUG_TRACE("existed animal: " << iter->second << ", name=" << animal->getName());
    } else {
        m_animals.insert({animal->getName(), animal});
        DEBUG_TRACE("insert animal: " << getAnimalCount(animal->getName()) << ", name=" << animal->getName());
    }
}

uint32_t Zookeeper::getAnimalCount(const std::string& name) {
    if (auto iter = m_counter.find(name); iter != m_counter.end()) {
        return iter->second;
    }
    return 0;
}

const std::shared_ptr<Animal>& Zookeeper::getAnimal(const std::string& name) {
    if (auto iter = m_animals.find(name); iter != m_animals.end()) {
        return iter->second;
    }
    return m_empty_animal;
}

int enable_shared_from_this_demo(int argc, char* argv[]) {
    auto keeper= make_shared<Zookeeper>();
    auto dog = make_shared<Animal>("dog", keeper);
    DEBUG_TRACE(". m_name=" << dog->getName());

    auto dog2 = dog->getThisEntity();
    DEBUG_TRACE(". m_name=" << dog2->getName());

    assert(dog == dog2);
    assert(!(dog < dog2 || dog2 < dog)); // ptr1 and ptr2 must share ownership

    keeper->addAnimal(dog);
    keeper->addAnimal(dog2);

    DEBUG_TRACE(keeper->getAnimalCount("dog"));
    auto search = keeper->getAnimal("dog");
    if (search)
        search->snarl();

    return 0;
}