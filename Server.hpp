#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>
class Server{
    public :
        std::string     servername;
        std::string     port;
        int             server_socket;
        struct addrinfo hint;
        struct addrinfo *res;

        Server(std::string serv, std::string port);
        ~Server();
        void    CreateSocket();
};