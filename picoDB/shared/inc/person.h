//
// Created by kimi on 14/06/15.
//

#ifndef PICODB_PERSON_H
#define PICODB_PERSON_H

#define LOG true
#define TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE       61
#define TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE    120
#define TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE     13

#include <iostream>
#include <string.h>
#include <vector>
#include "LockFile.h"
#include <iostream>
#include <sstream>

using namespace std;

class PersonRow {
private:
    //char nombre[TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE];
    //char direccion[TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE];
    //char telefono[TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE];

    string nombre;
    string direccion;
    string telefono;

public:
    PersonRow(std::string nombre, std::string direccion, std::string telefono);

    ~PersonRow();

    string getNombre();

    string getDireccion();

    string getTelefono();
};

class Person {
private:

    LockFile file;

public:
    Person(std::string path);

    ~Person();

    bool addPerson(std::string nombre, std::string direccion, std::string telefono);

    bool addPerson(PersonRow person);

    std::vector<PersonRow> getPersons();
};

#endif //PICODB_PERSON_H
