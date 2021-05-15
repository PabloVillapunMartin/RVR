#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <thread>

#define TAM_BUFFER 80

using namespace std;

class ThreadMessage {
public:
    ThreadMessage(int id_cliente) : _id_cliente(id_cliente) {} 
    ~ThreadMessage() {}

    void processMessages(){
        while(true){
            char buffer[TAM_BUFFER];

            memset((void *) buffer, 0, TAM_BUFFER);

            int bytes = recv(_id_cliente, (void *) buffer, TAM_BUFFER, 0);

            if(bytes == -1){
                cerr << "[recv] Error al recibir mensaje\n";
                return;
            }
            else if(bytes == 0){
                cout << "Conexion terminada del cliente " << _id_cliente << "\n";
                break;
            }

            send(_id_cliente, buffer, bytes, 0);
        }

        close(_id_cliente);
    }
private:
    int _id_cliente;
};

int main(int argc, char** argv){

    //Control de numero de argumentos de entrada
    if(argc != 3){
        cout << argc << " | El programa debe tener 2 argumentos\n";
        return -1;
    }

    int errorManagement = 0; //Variable para controlar errores en metodos

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errorManagement= getaddrinfo(argv[1], argv[2], &hints, &res);

    if(errorManagement != 0){
        cerr << "[getaddrinfo]: " << gai_strerror(errorManagement) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1){
        cerr << "[socket]: creacion socket\n";
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    //Esperando conexion
    listen(sd, 16);

    //Estructuras para informacion sobre el cliente
    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);

    while(t){
        //Aceptar conexion y recoger datos del cliente
        int id_cliente = accept(sd, &cliente, &clientelen);
        ThreadMessage* tm = new ThreadMessage(id_cliente);

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        //Obtener informacion de host y puertos
        getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        cout << "Conexion desde host " << host << ", Puerto: " << serv << ", ID cliente: " << id_cliente << '\n';

        thread([&]() { tm->processMessages();}).detach();
    }    

    close(sd);

    return 0;
}