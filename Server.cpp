#include "server.hpp"

Server::Server(std::string serv, std::string port)
{
    servername = serv;
    this->port = port;
}

Server::~Server()
{
//    std::cout << "Server Destructor Called !" << std::endl;
}

void    Server::CreateSocket()
{
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(servername.c_str(), port.c_str(), &hint, &res) != 0)
    {
        std::cerr << "getaddrinfo error !" << std::endl;
        exit(1);
    }
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) == -1)
    {
        std::cerr << "Couldn't perform binding !" << std::endl;
        close(server_socket);
    }

    if (listen(server_socket, 400) == -1)
    {
        std::cerr << "Couldn't perfom listening with the Socket !" << std::endl;
        close(server_socket);
    }
}
