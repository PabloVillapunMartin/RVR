#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <thread>

#define TAM_BUFFER 80
#define NUM_THREADS 10

using namespace std;

class ThreadMessage {
public:
    ThreadMessage(int sd) : _sd(sd) {} 
    ~ThreadMessage() {}

    void processMessages(){
        while(true){
            char buffer[TAM_BUFFER];

            memset((void *) buffer, 0, TAM_BUFFER);

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            struct sockaddr cliente;
            socklen_t clientelen = sizeof(struct sockaddr);

            int bytes = recvfrom(_sd, (void*) buffer, TAM_BUFFER, 0, &cliente, &clientelen);

            if(bytes == -1){
                cerr << "[recvfrom]\n"; 
                return;
            }

            int errorManagement = getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            if(errorManagement == -1){
                cerr << "[getnameinfo]\n";
            }

            cout << "[" << this_thread::get_id() << "] : " << bytes << " bytes de " << host << ":" << serv << '\n';

            char buffer_envio[TAM_BUFFER];

            memset((void *) buffer_envio, 0, TAM_BUFFER);   //Limpiar buffer de datos

            //Obtener tiempo del sistema
            time_t timer = time(NULL);
            struct tm* tim = localtime(&timer);

            switch (*buffer)
            {
                case 't':{  //Enviar hora minutos y segundos
                    strftime(buffer_envio, TAM_BUFFER, "%r", tim);
                    sendto(_sd, &buffer_envio, sizeof(time_t), 0, &cliente, clientelen);
                    break;
                }
                case 'd':{
                    strftime(buffer_envio, TAM_BUFFER, "%D", tim);
                    sendto(_sd, &buffer_envio, sizeof(time_t), 0, &cliente, clientelen);
                    break;
                }
                default:{
                    cout << "Comando no soportado\n";
                    break;
                }
            }

            sleep(3);
        }
    }
private:
    int _sd;
};

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

    ThreadMessage* threadMessages = new ThreadMessage(sd);

    for(int i = 0; i < NUM_THREADS; ++i){
        thread([&](){
            threadMessages->processMessages();
        }).detach();
        cout << "Creado thread " << i << '\n';
    }

    char c[10];
    while(c[0] != 'q') cin.getline(c, 10);

    cout << "cerrando servidor\n";

    close(sd);

    return 0;
}