#include <iostream>
#include <string>
#include "ref.h"


class Animal {
public:
    Animal(std::string name): _name(name) {
        std::cout << "Animal(\"" << _name << "\")\n";
    }
    
    virtual ~Animal() {
        std::cout << "~Animal(\"" << _name << "\")\n";
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


void test(ref<Animal> animal) {
    std::cout << "Testing:\n";
    std::cout << "- Method => \"" << animal->getName() << "\"\n";
    std::cout << "- typeid => \"" << animal.type().name() << "\"\n";
    std::cout << "- De-reference => " << *animal << "\n";
}


int main() {
    Cat kitten("Kitten");
    
    ref<Cat> pet_1 = noref &kitten;       // Don't use garbage collection.
    ref<Cat> pet_2 = new Cat("Kitty");    // Use garbage collection.
    
    ref<Animal> animal = pet_2;           // Up-cast.
    ref<Cat> cat = animal.cast<Cat>();    // Down-cast.
    
    test(pet_1);
    test(cat);
}
