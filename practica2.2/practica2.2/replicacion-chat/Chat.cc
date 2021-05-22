#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* bufferPointer = _data;

    memcpy(bufferPointer, &type, sizeof(uint8_t));
    bufferPointer += sizeof(uint8_t);

    memcpy(bufferPointer, &nick, 8 * sizeof(char));
    bufferPointer += 8 * sizeof(char);

    memcpy(bufferPointer, &message, 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* bufferPointer = _data;

    memcpy(&type, &bufferPointer, sizeof(uint8_t));
    bufferPointer += sizeof(uint8_t);

    mempcpy(&nick, &bufferPointer, 8 * sizeof(char));
    bufferPointer += 8 * sizeof(char);

    memcpy(&message, &bufferPointer, 80 * sizeof(char));

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage* message;
        Socket* clientSocket;

        if(socket.recv(*message, clientSocket) != 0){
            std::cout << "[socket.recv]\n";
            return;
        }

        switch (message->type)
        {
            case ChatMessage::MessageType::LOGIN :{
                if(isAlreadyRegistered(clientSocket)) break;

                std::unique_ptr<Socket> loginRequest(new Socket(*clientSocket));
                clients.emplace_back(std::move(loginRequest));

                std::cout << message->nick << " has joined\n";
                break;
            }
            case ChatMessage::MessageType::LOGOUT :{
                removeClient(clientSocket);
                std::cout << message->nick << " has disconnected\n";
                break;
            }
            case ChatMessage::MessageType::MESSAGE :{
                sendMessageToAllCLients(clientSocket, message);
                break;
            }
            default:
                std::cout << "Received unknown message type\n";
                break;
        }

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    }
}

bool ChatServer::isAlreadyRegistered(Socket* client){
    for(int i = 0; i < clients.size(); ++i){
        if(*clients[i].get() == *client) return true;
    }
    return false;
}

void ChatServer::removeClient(Socket* client){
    std::vector<std::unique_ptr<Socket>>::iterator it = clients.begin();

    while(it != clients.end()){
        if(*it->get() == *client) break;
        it++;
    } 

    if(it != clients.end()) clients.erase(it);
}

void ChatServer::sendMessageToAllCLients(Socket* senderClient, ChatMessage* message){
    for(int i = 0; i < clients.size(); ++i){
        if(*clients[i].get() == *senderClient) continue;
        socket.send(*message, *clients[i].get());
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

