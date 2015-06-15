//
// Created by kimi on 14/06/15.
//

#include "../inc/person.h"

Person::Person(std::string n, std::string d, std::string t) {

    if (LOG) {
        cout << "Creating object. {" << n << "\t" << d << "\t" << t << "}" << std::endl;
    }

    if (n.length() > TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE)
        throw ("nombre: " + n + " size " + to_string(n.length()) + " > " + to_string(TABLE_PERSON_COLUMN_NOMBRE_FIXED_SIZE));

    if (d.length() > TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE)
        throw ("dirección: " + d + " size " + to_string(d.length()) + " > " + to_string(TABLE_PERSON_COLUMN_DIRECCION_FIXED_SIZE));

    if (t.length() > TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE)
        throw ("telefono: " + t + " size " + to_string(t.length()) + " > " + to_string(TABLE_PERSON_COLUMN_TELEFONO_FIXED_SIZE));
}
