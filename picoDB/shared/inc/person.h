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
#include "LockFile.h"

using namespace std;

class Person {
private:

	LockFile file;

    char nombre[TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE];
    char direccion[TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE];
    char telefono[TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE];

public:
    Person(std::string path);
    ~Person();
    void addPerson(std::string nombre, std::string direccion, std::string telefono);
    std::string getPersons();
//    Person(std::string nombre, std::string direccion, std::string telefono);
};

#endif //PICODB_PERSON_H
