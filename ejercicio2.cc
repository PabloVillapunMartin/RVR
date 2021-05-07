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

    bool server_up = true;
    while(server_up){
        char buffer[TAM_BUFFER];

        memset((void *) buffer, 0, TAM_BUFFER);

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*) buffer, TAM_BUFFER, 0, &cliente, &clientelen);

        if(bytes == -1){
            cerr << "Error en recvfrom\n"; 
            return -1;
        }

        errorManagement = getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if(errorManagement == -1){
            cerr << "[getnameinfo]\n";
        }

        cout << bytes << " bytes de " << host << ":" << serv << '\n';

        char buffer_envio[TAM_BUFFER];

        memset((void *) buffer_envio, 0, TAM_BUFFER);   //Limpiar buffer de datos

        //Obtener tiempo del sistema
        time_t timer = time(NULL);
        struct tm* tim = localtime(&timer);

        switch (*buffer)
        {
            case 't':{  //Enviar hora minutos y segundos
                strftime(buffer_envio, TAM_BUFFER, "%r", tim);
                sendto(sd, &buffer_envio, sizeof(time_t), 0, &cliente, clientelen);
                break;
            }
            case 'd':{
                strftime(buffer_envio, TAM_BUFFER, "%D", tim);
                sendto(sd, &buffer_envio, sizeof(time_t), 0, &cliente, clientelen);
                break;
            }
            case 'q':{
                server_up = false;
                cout << "Cerrando servidor...\n";
                break;
            }
            default:{
                cout << "Comando no soportado\n";
                break;
            }
        }

    }

    close(sd);

    return 0;
}