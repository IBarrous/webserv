#include "Server.hpp"
#include "Client.hpp"
#include <fstream>
#include <sstream>
#include <csignal>
std::vector<Server> servers;
std::vector<Client> clients;

std::string intToString(int number)
{
    std::stringstream ss;
    ss << number;
    std::string strNumber = ss.str();
    return strNumber;
}

void    readFromFile(std::string file, std::string &str)
{
    std::ifstream inputFile(file.c_str());
    if (!inputFile) {
        std::cerr << "Failed to open the file." << std::endl;
        return ;
    }
    std::string line;
    while (std::getline(inputFile, line))
        str += line + "\n";
    inputFile.close();
}
void    createConnections(std::vector<Server>& servers)
{
        std::vector<Server>::iterator it = servers.begin();
        while (it != servers.end())
        {
            (*it).CreateSocket();
            it++;
        }
}
void sighandler(int signal)
{
    if (signal == SIGINT)
    {
        std::vector<Server>::iterator it = servers.begin();
        std::vector<Client>::iterator it1 = clients.begin();
        while (it1 != clients.end())
        {
            close((*it1).client_socket);
            it1++;
        }
        while (it != servers.end())
        {
            close((*it).server_socket);
            it++;
        }
        exit(1);
    }
}
int main()
{
    signal(SIGINT, sighandler);
    servers.push_back(Server("localhost", "8080"));
    servers.push_back(Server("localhost", "9090"));
    createConnections(servers);
    fd_set rd, wr;
    while (1)
    {
        FD_ZERO(&rd);
        FD_ZERO(&wr);
        int maxSocket = -1;
        std::vector<Server>::iterator it = servers.begin();
        while (it != servers.end())
        {
            FD_SET((*it).server_socket, &rd);
            if ((*it).server_socket > maxSocket)
                maxSocket = (*it).server_socket;
            it++;
        }
        std::vector<Client>::iterator it1 = clients.begin();
        while (it1 != clients.end())
        {
            FD_SET((*it1).client_socket, &rd);
            FD_SET((*it1).client_socket, &wr);
            if ((*it1).client_socket > maxSocket)
                maxSocket = (*it1).client_socket;
            it1++;
        }
        if (select(maxSocket + 1, &rd, &wr, NULL, NULL) == -1)
        {
            std::cerr << "Error in Select !" << std::endl;
        }
        else
        {
            int i = 0;
            it = servers.begin();
            while (it != servers.end())
            {
                if (FD_ISSET((*it).server_socket, &rd))
                {
                    Client newClient;
                    newClient.client_socket = accept((*it).server_socket, NULL, NULL);
                    if (newClient.client_socket == -1)
                    {
                        std::cerr << "Failed to accept client connection" << std::endl;
                        continue;
                    }
                    newClient.server_id = i;
                    clients.push_back(newClient);
                    /*std::cout << "Hello client number " \
                    << newClient.client_socket << " from " \
                    << servers[newClient.server_id].servername << ":" << servers[newClient.server_id].port << std::endl;*/
                }
                it++;
                i++;
            }
            it1 = clients.begin();
            while (it1 != clients.end())
            {
                if (FD_ISSET((*it1).client_socket, &rd))
                {
                    int r = read((*it1).client_socket, (*it1).data, sizeof((*it1).data));
                    if (r < 0)
                    {
                        std::cerr << "An error has occured during reading request from a client" << std::endl;
                    }
                    else if (!r)
                    {
                        std::cout << "nothing to read anymore !" << std::endl;
                    }
                    else
                    {
                        std::cout << (*it1).data << std::endl;
                    }
                }
                else if (FD_ISSET((*it1).client_socket, &wr))
                {
                    std::string responseBody;
                    readFromFile("index.html", responseBody);
                    std::string responseHeader = "HTTP/1.1 200 OK\r\n";
                    responseHeader += "Content-Type: text/html\r\n";
                    responseHeader += "Content-Length: " + intToString(responseBody.length()) + "\r\n";
                    responseHeader += "\r\n";
                    responseHeader += responseBody;
                    ssize_t bytesSent = send((*it1).client_socket, responseHeader.c_str(), responseHeader.length(), 0);
                    if (bytesSent == -1) {
                        std::cerr << "Failed to send response header." << std::endl;
                    } else {
                        std::cout << "Sent " << bytesSent << " bytes of response header." << std::endl;
                    }
                    close((*it1).client_socket);
                    (*it1).client_socket = -1;
                    if (it1 != clients.end()) {
                        clients.erase(it1);
                        it1--;
                    }
                }
                it1++;
            }
        }
    }
}