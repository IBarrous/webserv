#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
class Client{
    public :
        int     client_socket;
        int     server_id;
        char    data[9000];
        Client();
        Client(int sock);
        ~Client();
};