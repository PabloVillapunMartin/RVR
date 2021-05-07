#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char** argv){

    if(argc != 3){
        cout << argc << " | El programa debe tener 2 argumentos\n";
        return -1;
    }

    int errorManagement = 0; //Variable para controlar errores en metodos

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

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

    errorManagement = bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while(true){
        char buffer[80];

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*) buffer, 80, 0, &cliente, &clientelen);

        if(bytes == -1){
            cerr << "Error en recvfrom\n"; 
            return -1;
        }

        errorManagement = getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if(errorManagement == -1){
            cerr << "[getnameinfo]\n";
        }

        cout << "Mensaje recibido -> " << "Host: " << host << ", Puerto: " << serv << '\n';
        cout << "Contenido: " << buffer;
    }

    close(sd);

    return 0;
}