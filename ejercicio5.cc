#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

#define TAM_BUFFER 80

using namespace std;

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

    //Estructuras para informacion sobre el cliente
    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    //Obtener informacion de host y puertos
    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    //Conectarse al servidor
    errorManagement = connect(sd, res->ai_addr, res->ai_addrlen);

    if(errorManagement == -1){
        cerr << "[connect] Error al conectar al servidor\n";
        return -1;
    }

    freeaddrinfo(res);

    cout << "Conectado al servidor " << host << " en puerto " << serv << '\n';

    bool loop_client = true;
    while(loop_client){
        char buffer_envio[TAM_BUFFER];
        char buffer_recivo[TAM_BUFFER];

        memset((void *) buffer_recivo, 0, TAM_BUFFER);
        memset((void *) buffer_envio, 0, TAM_BUFFER);

        cin.getline(buffer_envio, TAM_BUFFER);

        errorManagement = send(sd, &buffer_envio, strlen(buffer_envio) + 1, 0);

        if(errorManagement == -1){
            cerr << "[send] Error al enviar el mensaje\n";
            return -1;
        }
        if(buffer_envio[0] == 'Q') loop_client = false;

        int bytes = recv(sd, (void *) buffer_recivo, TAM_BUFFER, 0);

        if(bytes == -1){
            cerr << "[recv] Error al recibir mensaje\n";
            return -1;
        }

        cout << buffer_recivo << '\n';
    }

    cout << "Conexion finalizada con el servidor\n";

    close(sd);

    return 0;
}