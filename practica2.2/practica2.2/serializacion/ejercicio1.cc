#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);

        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* dataPointer = _data;

        memcpy(dataPointer, name, MAX_NAME * sizeof(char));
        dataPointer += MAX_NAME * sizeof(char);

        memcpy(dataPointer, &pos_x, sizeof(int16_t));
        dataPointer += sizeof(int16_t);

        memcpy(dataPointer, &pos_y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char* dataPointer = data;

        memcpy(name, dataPointer, MAX_NAME * sizeof(char));
        dataPointer += MAX_NAME * sizeof(char);

        memcpy(&pos_x, dataPointer, sizeof(int16_t));
        dataPointer += sizeof(int16_t);

        memcpy(&pos_y, dataPointer, sizeof(int16_t));
        dataPointer += sizeof(int16_t);

        return 0;
    }


public:
    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];

    int16_t pos_x;
    int16_t pos_y;
};

int main(int argc, char **argv)
{
    Jugador playerone("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    std::cout << "Serializing player...\n";
    playerone.to_bin();

    // 2. Escribir la serialización en un fichero
    char* fileName = "./jugadorSerializado";
    int fd = open(fileName, O_CREAT | O_TRUNC | O_RDWR, 0666);

    write(fd, playerone.data(), playerone.size());

    close(fd);

    // 3. Leer el fichero
    std::cout << "Reading serialized file...\n";
    fd = open(fileName, O_RDONLY, 0666);

    char data[playerone.size()];
    read(fd, data, playerone.size());

    close(fd);

    // 4. "Deserializar" en one_r
    std::cout << "Deserializing data...\n";
    playerone.from_bin(data);

    // 5. Mostrar el contenido de one_r
    std::cout << "Player name: " << playerone.name << '\n';
    std::cout << "Player x_pos: " << playerone.pos_x << '\n';
    std::cout << "Player y_pos: " << playerone.pos_y << '\n';

    return 0;
}

