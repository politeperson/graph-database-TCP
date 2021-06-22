#ifndef TOOLSCLIENT_H
#define TOOLSCLIENT_H

#include "string.h"

void separateNodeAttributesRelation(int size_data, char *data[], string &name, map<string, string> &atributos, vector<string> &relations)
{
    //C "JULIO" ["EDAD" "50"] ["NodoB" "NodoC"]
    //C "JULIO" [] ["NodoB" "NodoC"]
    //C "JULIO" ["EDAD" "50"] []
    bool statusAttributes = false;
    bool statusRelations = false;
    //cout << "Separate" << endl;

    name = data[2];

    if (data[3][0] == '[')
    {
        if (data[3][1] != ']')
            statusAttributes = true;
    }

    //Instanciamos por si no hay atributos
    //int i;
    int i = 4;
    if (statusAttributes)
    {
        // Guardar el primer atributo
        string name_attribute = data[3];
        name_attribute = name_attribute.substr(1, name_attribute.size());
        string value_attribute = data[4];
        if (value_attribute.back() == ']')
        {
            value_attribute.pop_back();
            statusAttributes = false;
            i = 5;
        }
        atributos[name_attribute] = value_attribute;
    }

    //cout << "Estado Attributo: " << statusAttributes << endl;
    //Guardar Atributos
    if (statusAttributes)
    {
        for (i = 5; statusAttributes && i < size_data; i += 2)
        {
            string value_attribute = data[i + 1];
            if (value_attribute.back() == ']')
            {
                value_attribute.pop_back();
                statusAttributes = false;
            }
            atributos[data[i]] = value_attribute;
        }
    }

    //Guardar Relaciones
    //cout << "Indice:" << i << endl;
    //cout << "Size:" << size_data << endl;
    if (i != size_data)
    {
        if (data[i][0] == '[')
        {
            if (data[i][1] != ']')
                statusRelations = true;
        }
        //cout << "Estado: " << statusRelations << endl;
        if (statusRelations)
        {
            string node_relations = data[i];
            node_relations = node_relations.substr(1, node_relations.size());

            if (node_relations.back() == ']')
            {
                node_relations.pop_back();
                statusRelations = false;
            }
            relations.push_back(node_relations);
        }

        for (i = i + 1; statusRelations && i < size_data; i += 1)
        {
            string node_relations = data[i];
            if (node_relations.back() == ']')
            {
                node_relations.pop_back();
                statusAttributes = false;
            }
            relations.push_back(node_relations);
        }
    }

}

#endif