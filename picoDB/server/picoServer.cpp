#include <iostream>

#include "../shared/inc/person.h"

using namespace std;

int main() {
    cout << "picoDB Server" << endl;

    try {
        Person* p = new Person("Hola","Direccion","123456789123456789");

        delete p;
    } catch (std::string error){
        cerr << "Exception: " << error << std::endl;
    }

    return 0;
}