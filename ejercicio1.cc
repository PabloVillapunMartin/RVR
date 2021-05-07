#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char** argv){

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], NULL, &hints, &res);

    if(rc != 0){
        cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    for(addrinfo* i = res; i != NULL; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        cout << host << "     " << i->ai_family << "     " << i->ai_socktype << '\n';
    }

    freeaddrinfo(res);

    return 0;
}