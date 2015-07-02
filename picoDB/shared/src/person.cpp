//
// Created by kimi on 14/06/15.
//

#include "person.h"

PersonRow::PersonRow(std::string nombre, std::string direccion, std::string telefono) {
    this->nombre = nombre;
    this->direccion = direccion;
    this->telefono = telefono;
}

PersonRow::~PersonRow() { }

string PersonRow::getNombre() {
    return nombre;
}

string PersonRow::getDireccion() {
    return direccion;
}

string PersonRow::getTelefono() {
    return telefono;
}


// ##############################

Person::Person(std::string path) : file(path) { }

Person::~Person() { }

bool Person::addPerson(PersonRow person) {
    return addPerson(person.getNombre(), person.getDireccion(), person.getTelefono());
}

bool Person::addPerson(std::string n, std::string d, std::string t) {
    if (n.length() > TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE)
        throw ("nombre " + n + " size " + to_string(n.length()) + " > " +
               to_string(TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE));

    if (d.length() > TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE)
        throw ("dirección " + d + " size " + to_string(d.length()) + " > " +
               to_string(TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE));

    if (t.length() > TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE)
        throw ("telefono " + t + " size " + to_string(t.length()) + " > " +
               to_string(TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE));

    std::string buffer = n + "," + d + "," + t + "\n";
    file.tomarLockEscritura();
    file.escribir(buffer.c_str(), buffer.size());
    file.liberarLock();

    return true;
}

std::vector<std::string> &splitFields(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> splitFields(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitFields(s, delim, elems);
    return elems;
}

std::vector<PersonRow> Person::getPersons() {
    std::vector<PersonRow> persons;

    ssize_t MAX_BUFFER = 256;
    size_t offset = 0;
    size_t bytes_read;
    int i;
    file.tomarLockLectura();
    file.setInit();
    do {
        char *buffer;
        //memset(buffer, 0, MAX_BUFFER);
        std::string buffer_str = "";

        /* Read the next line's worth of bytes.  */
        /*
        bytes_read = file.leer(buffer, sizeof(buffer));

        buffer_str.append(buffer);
        */
        bytes_read = file.leerLinea(&buffer, MAX_BUFFER);

        if (bytes_read > 0) {
            buffer_str.append(buffer);

            std::vector<std::string> fieldsvector = splitFields(buffer_str, ',');

            if (fieldsvector.size() == 3) {
                string nombre = fieldsvector[0];
                string direccion = fieldsvector[1];
                string telefono = fieldsvector[2];
                PersonRow p(nombre, direccion, telefono);

                persons.push_back(p);
            }
        }
        /* Keep count of our position in the file.  */
        offset += bytes_read;

        delete buffer;
    }
    while (bytes_read > 0);
    file.liberarLock();

    return persons;
}
