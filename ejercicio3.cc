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

    if(argc != 4){
        cout << argc << " | El programa debe tener 3 argumentos\n";
        return -1;
    }

    int errorManagement = 0; //Variable para controlar errores en metodos

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errorManagement= getaddrinfo(argv[1], argv[2], &hints, &res);

    //Control de errores
    if(errorManagement != 0){
        cerr << "[getaddrinfo]: " << gai_strerror(errorManagement) << std::endl;
        return -1;
    }

    //Creacion de socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);

    //Control de errores
    if(sd == -1){
        cerr << "[socket]: creacion socket\n";
        return -1;
    }

    char buffer[TAM_BUFFER];

    memset((void *)buffer, 0, TAM_BUFFER);

    //Enviar datos al servidor
    errorManagement = sendto(sd, &argv[3], sizeof(char) + 1, 0, res->ai_addr, res->ai_addrlen);

    //Control de errores
    if(errorManagement == -1){
        cerr << "[sendto] Error al enviar el mensaje : " << gai_strerror(errorManagement) << std::endl;
        return -1;
    }

    //Recibir respuesta del servidor
    int bytes = recvfrom(sd, (void *) buffer, TAM_BUFFER, 0, res->ai_addr, &res->ai_addrlen);

    //Control de errores
    if(bytes == -1){
        cerr << "[recvfrom] Error al recibir el mensaje del servidor : " << gai_strerror(errorManagement) << std::endl;;
        return -1;
    }

    //Mostrar la respuesta del servidor
    cout << buffer;

    freeaddrinfo(res);
    close(sd);

    return 0;
}