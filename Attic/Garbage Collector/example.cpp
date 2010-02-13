#include <iostream>
#include <string>
#include "ref.h"


class Animal {
public:
    Animal(std::string name): _name(name) {
        std::cout << "Animal()\n";
    }
    
    virtual ~Animal() {
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


class Cat: public Animal {
public:
    Cat(std::string name): Animal(name) {
    }
};


int main() {
    ref<Cat> kitty = new Cat("Kitty");
    ref<Animal> animal = kitty;           // Up-cast.
    ref<Cat> cat = animal.cast<Cat>();    // Down-cast.
    
    std::cout << "- Down-cast " << (cat == NULL ? "failed" : "ok") << ".\n";
    std::cout << "- Method => \"" << cat->getName() << "\"\n";
    std::cout << "- typeid => \"" << cat.type().name() << "\"\n";
    std::cout << "- De-reference => " << *cat << "\n";
}
