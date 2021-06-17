#include "DGDB.h"
#include "tools.h"

using namespace std;

void DGDB::runConnection(int Pconnection)
{
  int n;
  int s, ss;
  char buffer[1024];
  char bufferB[1024];
  connections[Pconnection] = "";
  std::string data;
  bool existeRelaciones = false;
  bool existeAtributos = false;
  std::map<std::string, std::string> storageAttrValue;

  std::cout << "wwww" << std::endl;
  while (server || repository)
  {
    n = read(Pconnection, buffer, 1);
    if (n < 0)
      perror("ERROR reading from socket");
    else if (n == 0)
    {
    }
    else if (n > 0)
    {
      printf("%s\n", buffer);
    }
    if (buffer[0] == 'C')
    {
      std::cout << "Action:C\n";
      n = read(Pconnection, buffer, 3);
      if (n < 3)
      {
        perror("ERROR reading size\n");
      }
      else
      {
        buffer[n + 1] = '\0';
        //s = name_node_size + 2
        //Buffer = 005
        //5+2 = 7
        //2B <- number_of_attributes(1B) y number_of_relations(1B)
        s = atoi(buffer) + 2;
      }
      //buffer = julio01
      n = read(Pconnection, buffer, s);
      if (n < s)
      {
        perror("ERROR reading size\n");
      }
      else
      {
        int r;
        //s -> como indice ?
        if (buffer[s - 1] == '0') // no existen relaciones
        {
          std::cout << "No se envio relaciones" << std::endl;
          buffer[s - 1] = '\0';
        }
        // calcula la cantidad de relaciones
        else// if (buffer[s - 1] - '0' > 0)
        { // exite almenos una relacion
          existeRelaciones = true;
          n = read(Pconnection, bufferB, 3);
          bufferB[3] = '\0';
          ss = atoi(bufferB);
          n = read(Pconnection, bufferB, ss);
          bufferB[n] = '\0';

          std::cout << "nodeB:" << bufferB << std::endl;
        }

        if (buffer[s - 2] == '0') // no existen atributos
        {
          std::cout << "No se envio atributos" << std::endl;
          buffer[s - 2] = '\0';
        }
        //existen atributos: decodificar atributos
        else
        {
          std::size_t number_atributes = buffer[s - 2] - '0';
          char attrBuffer[1024], valueBuffer[1024];
          
          for(std::size_t i = 0; i < number_atributes; ++i){
            attrBuffer[read(Pconnection, attrBuffer, 3)] = '\0';
            uint8_t name_attribute_size = atoi(attrBuffer);
            attrBuffer[read(Pconnection, attrBuffer, name_attribute_size)] = '\0';
        
            valueBuffer[read(Pconnection, valueBuffer, 3)] = '\0';          
            uint8_t val_attribute_size = atoi(valueBuffer);
            valueBuffer[read(Pconnection, valueBuffer, val_attribute_size)] = '\0';  
          
            storageAttrValue[std::string(attrBuffer)] = std::string(valueBuffer);
          }
          
        }

        //--------------------------------------


        //std::cout << "data:[" << buffer << "]" << std::endl;
        //buffer = julio01
        data = buffer;
        data.pop_back();
        data.pop_back();
        // data tiene el nombre del nodo

        if (socketRepositories.size() > 1)
        {
          // enviar al  Repository
          // determinar que repository
          if (socketRepositories.size() < 2)
          {
            std::cout << "Repository has not been attached." << std::endl;
            return;
          }
          else
            r = buffer[0] % (socketRepositories.size() - 1);
          r++;
          std::cout << "xxxx--:" << socketRepositories.size() << std::endl;
          std::cout << "xxxx-r:" << r << std::endl;
          std::cout << "xxxx-s:" << socketRepositories[r] << std::endl;
          std::cout << "xxxx-d:" << data << std::endl;
          if (existeRelaciones && !existeAtributos)
          {
            createRelation(data, bufferB, socketRepositories[r]);
          }
          else if (existeRelaciones && existeAtributos)
          {
            //createAtri();
          }
          else if (!existeRelaciones && existeAtributos)
          {
            
            createNodeAttrite(data, socketCliente, storageAttrValue);
          }
          else if (!existeRelaciones && !existeAtributos)
          {
            createNode(data, socketRepositories[r]);
          }

          std::cout << "xxxx" << std::endl;
        }
        else if (repository)
        {
          std::cout << "Store:" << data << "-" << bufferB << std::endl;
        }
      }
    }
    if (buffer[0] == 'R')
    {
    }
    if (buffer[0] == 'U')
    {
    }
    if (buffer[0] == 'D')
    {
    }
    if (buffer[0] == 'R')
    {
      n = read(Pconnection, buffer, 21);
      if (n < 21)
      {
        perror("ERROR reading size\n");
      }
      else
      {
        char vIp[17];
        char vPort[6];
        int vPort_int;
        char *pbuffer;
        strncpy(vPort, buffer, 5);
        vPort_int = atoi(vPort);
        pbuffer = &buffer[5];
        strncpy(vIp, pbuffer, 16);
        buffer[16] = '\0';
        string vIp_string = vIp;
        trim(vIp_string);
        connMasterRepository(vPort_int, vIp_string);
        if (repository)
        {
          socketRepositories.push_back(socketRepository);
          std::cout << "Repository registed." << std::endl;
        }
        else
          perror("ERROR no se pudo registrar Repositorio\n");
      }
    }

    //n = write(ConnectFD,"I got your message",18);
    //if (n < 0) perror("ERROR writing to socket");
    /* perform read write operations ... */
  }
  connections.erase(Pconnection);
  shutdown(Pconnection, SHUT_RDWR);
  close(Pconnection);
}

void DGDB::runMainServer()
{
  for (;;)
  {
    int newConnection = accept(socketServer, NULL, NULL);
    if (0 > newConnection)
    {
      perror("error accept failed");
      close(newConnection);
      exit(EXIT_FAILURE);
    }
    std::thread(&DGDB::runConnection, this, newConnection).detach();
  }
}

void DGDB::runServer()
{
  if (mode == 'S')
  {
    std::thread runThread(&DGDB::runMainServer, this);
    runThread.join();
  }
  else if (mode == 'R')
  {
    std::cout << "RRRRR" << std::endl;
    std::thread runThread(&DGDB::runRepository, this);
    runThread.join();
  }
}
void DGDB::setClient()
{
  int Res;
  socketCliente = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketCliente)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }
  //memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(port);
  Res = inet_pton(AF_INET, ip.c_str(), &stSockAddr.sin_addr);
  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(socketCliente);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(socketCliente);
    exit(EXIT_FAILURE);
  }
  Res == connect(socketCliente, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  if (Res == -1)
  {
    perror("connect failed");
    close(socketCliente);
    exit(EXIT_FAILURE);
  }
  connection = 1;
}
void DGDB::closeClient()
{
  connection = 0;
  shutdown(socketCliente, SHUT_RDWR);
  close(socketCliente);
}

void DGDB::setServer()
{
  int Res;
  socketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketServer)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(port);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;
  Res = bind(socketServer, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  if (-1 == Res)
  {
    perror("error bind failed");
    close(socketServer);
    exit(EXIT_FAILURE);
  }
  Res = listen(socketServer, 10);
  if (-1 == Res)
  {
    perror("error listen failed");
    close(socketServer);
    exit(EXIT_FAILURE);
  }
  server = 1;
}

void DGDB::closeServer()
{
  server = 0;
  close(socketServer);
}

void DGDB::setRepository()
{
  std::cout << "setRepository" << std::endl;
  int Res;
  socketRepository = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketRepository)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(port);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;
  Res = bind(socketRepository, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  if (-1 == Res)
  {
    perror("error bind failed");
    close(socketServer);
    exit(EXIT_FAILURE);
  }
  Res = listen(socketRepository, 10);
  if (-1 == Res)
  {
    perror("error listen failed");
    close(socketRepository);
    exit(EXIT_FAILURE);
  }

  registerRepository();
  repository = 1;
  mode = 'R';
}

void DGDB::setNode(string name)
{
  createNode(name, socketCliente);
}

void DGDB::setRelation(string nameA, string nameB)
{
  createRelation(nameA, nameB, socketCliente);
}
void DGDB::createRelation(string nameA, string nameB, int conn ){

  //string tmp = nameA + "-" + nameB;
  //int n = write(socketCliente,tmp.c_str(),tmp.length());

  /*
int action; // 1B CRUD                  C
int name_node_size;// 3B                3
char name_node[255];// VB               15151515 (DNI)
int number_of_attributes; // 2B         0
int number_of_relations;  // 3B         1

// relations are optional
int node_relation_size;  // 3B
char node_relations[255];// VB

*/

  char tamano[4];
  sprintf(tamano, "%03d", nameA.length());
  string buffer;
  string tmp = tamano;
  buffer = "C" + tmp + nameA + "01";

  sprintf(tamano, "%03d", nameB.length());
  tmp = tamano;
  buffer = buffer + tmp + nameB;
  //C004julio01004UCSP

  int n;
  //std::cout << "*" << buffer.c_str() << "*" << "std::endl";
  n = write(conn, buffer.c_str(), buffer.length());
  if (n < 0)
  {
    perror("error listen failed");
    close(conn);
    exit(EXIT_FAILURE);
  }
  else if (n > 0 && n != buffer.length())
  {
    /* code */
    perror("error listen failed\n");
  }
}

/// Protocolo

void DGDB::createNode(string name, int conn)
{
      /*
    int action; // 1B CRUD                  C
    int name_node_size;// 3B                3
    char name_node[255];// VB               15151515 (DNI)
    int number_of_attributes; // 2B         1
    int number_of_relations;  // 3B         0
    */
  //name=julio
  char tamano[4];
  sprintf(tamano, "%03d", name.length());
  string buffer; 
  string tmp = tamano; //005
  buffer = "C" + tmp + name + "00";
  int n;
  //std::cout << "*" << buffer.c_str() << "*" << "std::endl";
  n = write(conn, buffer.c_str(), buffer.length());
  if (n < 0)
  {
    perror("error listen failed");
    close(socketCliente);
    exit(EXIT_FAILURE);
  }
  else if (n > 0 && n != buffer.length())
  {
    /* code */
  }
}

 void DGDB::createNodeAttrite(string name,int conn, const std::map<std::string, std::string>& storageAttrValue)
 {
   // attributes are optional  
      /*int name_attribute_size;  //  3B        13
      char name_attribute[255]; //  VB        Primer Nombre
      int value_attribute_size; //  3B        5
      char value_attribute[255]; // VB        Julio*/
    char tamano[4];
    sprintf(tamano, "%03d", name.length());
    string msg; 
    string tmp = tamano; //005
    msg = "C" + tmp + name + "0" + std::to_string(storageAttrValue.size());
    
   //Concatenar msg=C005Julios02DNI7219..
    for (auto item : storageAttrValue)
      msg += fixToBytes(item.first, 3) + item.first + fixToBytes(item.second, 3) + item.second;
    
    int n;
    std::cout << ">> Paquete :" << msg << '\n';

    n = write(conn, msg.c_str(), msg.length());
    if (n < 0)
    {
      perror("error listen failed");
      close(socketCliente);
      exit(EXIT_FAILURE);
    }
    else if (n > 0 && n != msg.length())
    {
      /*Code zzz olor a mierdaaaaaa*/
    }
 }

void DGDB::registerRepository()
{

  /*
int action; // X
int port; // 5B
char ip[16] VB
*/
  std::cout << ip << std::endl;
  string buffer;
  char vip[17];
  char vport[6];
  sprintf(vport, "%05d", port);
  vport[5] = '\0';
  sprintf(vip, "%016s", ip.c_str());
  std::cout << vip << std::endl;
  string sport = vport;
  string sip = vip;
  buffer = "R" + sport + sip;
  std::cout << "*" << buffer.c_str() << "*" << std::endl;
  // Set conn to Main
  int Res;
  int socketMain = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketMain)
  {
    std::cout << "cannot create socket" << std::endl;
    exit(EXIT_FAILURE);
  }
  //memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
  std::cout << "*1*" << std::endl;
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(mainPort);
  Res = inet_pton(AF_INET, mainIp.c_str(), &stSockAddr.sin_addr);
  if (0 > Res)
  {
    std::cout << "error: first parameter is not a valid address family" << std::endl;
    close(socketMain);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    std::cout << "char string (second parameter does not contain valid ipaddress" << std::endl;
    close(socketMain);
    exit(EXIT_FAILURE);
  }
  std::cout << "*2*" << std::endl;
  Res == connect(socketMain, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  if (Res == -1)
  {
    std::cout << "connect failed" << std::endl;
    close(socketMain);
    exit(EXIT_FAILURE);
  }
  //
  std::cout << "*3*" << std::endl;
  int n = write(socketMain, buffer.c_str(), buffer.length());
  std::cout << "*4*" << std::endl;
  if (n < 0)
  {
    std::cout << "error listen failed" << std::endl;
    close(socketMain);
    exit(EXIT_FAILURE);
  }
  else if (n > 0 && n != buffer.length())
  {
    std::cout << "Registing Repository:[" << buffer << "]" << std::endl;
  }
  std::cout << n << std::endl;
}

void DGDB::runRepository()
{

  for (;;)
  {
    int newConnection = accept(socketRepository, NULL, NULL);
    if (0 > newConnection)
    {
      perror("error accept failed");
      close(newConnection);
      exit(EXIT_FAILURE);
    }
    std::thread(&DGDB::runConnection, this, newConnection).detach();
  }
}
void DGDB::connMasterRepository(int pPort, string pIp)
{
  std::cout << pPort << "-" << pIp << std::endl;
  int Res;
  socketRepository = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (-1 == socketRepository)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }
  //memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(pPort);
  Res = inet_pton(AF_INET, pIp.c_str(), &stSockAddr.sin_addr);
  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(socketRepository);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(socketRepository);
    exit(EXIT_FAILURE);
  }
  Res == connect(socketRepository, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  if (Res == -1)
  {
    perror("connect failed");
    close(socketRepository);
    exit(EXIT_FAILURE);
  }
  repository = 1;
}