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

        int         client_socket;
        int         server_id;
        char        data[100];
        int         if_header;
        std::string datastr;
        std::string response;
        std::string boundary;
        int         contentlength;
        int         length;
        std::string filename;
        int         getleftdata;
        std::string location;
        std::string method;
        Client();
        Client(int sock);
        void    findmethod();
        ~Client();
};