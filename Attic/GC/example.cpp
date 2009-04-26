#include <iostream>
#include <string>
#include "GC.h"

using GC::ref;


class Animal {
public:
    Animal(std::string name) : _name(name) {
        std::cout << "Animal()\n";
    }
    
    ~Animal() {
        std::cout << "~Animal()\n";
    }
    
    std::string getName() const {
        return _name;
    }
    
    friend std::ostream& operator <<(std::ostream& out, const Animal& animal) {
        out << "Animal \"" << animal.getName() << '"';
        return out;
    }
    
private:
    std::string _name;
};


class Cat : public Animal {
public:
    Cat(std::string name) : Animal(name) {
    }
};


int main() {
    ref<Cat> cat = new Cat("Kitty");
    
    // Create another reference to the same object.
    ref<Animal> animal = cat;
    
    std::cout << "- Method => " << animal->getName() << "\n";
    std::cout << "- typeid => " << animal.type().name() << "\n";
    std::cout << "- De-reference => " << *animal << "\n";
    
    // No delete needed here.
}
