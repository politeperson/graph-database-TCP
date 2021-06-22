#include "DGDB.h"
#include "toolsClient.h"

using namespace std;

int main(int argc, char *argv[])
{
    DGDB db;
    // Configuracion el estado del programa
    // C -> Cliente
    // S -> Servidor
    // R -> Repositorio (Slave)
    if (argv[1][0] == 'C')
    {
        db.setMode('C');
        db.setClient();

        std::string nodo;
        std::map<std::string, std::string> atributos;
        std::vector<std::string> relations;

        separateNodeAttributesRelation(argc, argv, nodo, atributos, relations);
        
        db.setCreate(nodo,atributos,relations);
        
        //Impresion de los parametros por consola
        // cout << nodo << endl;
        // cout << "Atributos:" << endl;
        // for (auto item : atributos)
        //     cout << "    " << item.first << "     " << item.second << endl;
        // cout << "Relaciones:" << endl;
        // for (auto item : relations)
        //     cout << "    " << item << endl;
    }
    //Poner el programa en modo servidor
    else if (argv[1][0] == 'S')
    {
        db.setMode('S');
        db.setServer();
        db.runServer();
    }
    else if (argv[1][0] == 'R')
    {
        db.setPort(atoi(argv[2]));
        db.setIp("127.0.0.1");
        db.setMainIp("127.0.0.1");
        db.setMainPort(50000);
        db.setMode('R');
        db.setRepository();
        db.runServer();
    }
}
