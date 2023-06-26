#include "Server.hpp"
#include "Client.hpp"
#include <fstream>
#include <sstream>
#include <csignal>

void removeLastLine(std::string boundary, int& contentlength, std::string& str) {
    std::size_t f = str.find_last_of(boundary);
    if (f != std::string::npos)
    {
        std::size_t d = str.find_last_of("\r\n", f);

        if (d != std::string::npos) {    
            contentlength -= str.length() - d;
            str = str.substr(0, str.length() - d);
            //str.erase(lastLineBreakPos);
        }
    }
}

bool checkFileExistence(std::string str)
{
    return (access(str.c_str(), F_OK) == 0);
}
void    fillFile(std::string boundary, int contentlength, std::string& filename, std::string content)
{
   // std::stringstream ss;
    removeLastLine(boundary, contentlength, content);
    //removeLastLine(contentlength, content);
    std::cout << "<<" << content << "     xdlol>>" << std::endl;
    std::cout << "content size : " << contentlength << std::endl;
    std::cout << "File size : " << content.size() << std::endl;
    //ss << content;
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (file.is_open())
    {
        file.write(content.c_str(), contentlength);
        /*std::string line;
        while (std::getline(ss, line) && line.find(boundary) == std::string::npos)
            file << line;*/
    }
}

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

/*void sighandler(int signal)
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
}*/
int main()
{
    //exit(1);
    std::vector<Server> servers;
    std::vector<Client> clients;
   // signal(SIGINT, sighandler);
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
        //add new client to the list 
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
            return 1;
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
                    std::cout << "Hello client number " \
                    << newClient.client_socket << " from " \
                    << servers[newClient.server_id].servername << ":" << servers[newClient.server_id].port << std::endl;
                }
                it++;
                i++;
            }
            it1 = clients.begin();
            while (it1 != clients.end())
            {
                if (FD_ISSET((*it1).client_socket, &rd))
                {    
                    //std::cout << "Hiiii" << std::endl;
                    int r = read((*it1).client_socket, (*it1).data, sizeof((*it1).data));
                    if (r <= 0)
                    {
                        std::cerr << "An error has occured during reading request from a client" << std::endl;
                        close((*it1).client_socket);
                        (*it1).client_socket = -1;
                        if (it1 != clients.end()) {
                            clients.erase(it1);
                            it1--;
                        }
                    }
                    else
                    {
                        (*it1).data[r] = '\0';
                        if ((*it1).method.empty())
                            (*it1).datastr += (*it1).data;
                        else
                        {
                           // std::cout << "Hello from inside" << std::endl;
                            (*it1).length += r;
                            if ((*it1).length+1 > (*it1).contentlength)
                            {
    
                                //std::cout << "Hello from outside" << std::endl;
                                int x = (*it1).contentlength - ((*it1).length - r);
                                (*it1).data[x] = '\0';
                                (*it1).response +=  std::string((*it1).data, x);        
                                (*it1).if_header = 1;
                                size_t t = (*it1).response.find("filename=\"");
                                if (t != std::string::npos)
                                {
                                    t += 10;
                                    int p = (*it1).response.find("\"", t);
                                    (*it1).filename = (*it1).response.substr(t, p - t);
                                }
                                int u = (*it1).response.find("\r\n\r\n");
                                u += 4;
                                std::cout << "Content : " << (*it1).contentlength << std::endl;
                                std::cout << "Content : " << (*it1).length << std::endl;
                                std::cout << "Content : " << sizeof((*it1).response) << std::endl;
                                (*it1).length -= u;
                                fillFile((*it1).boundary, (*it1).length, (*it1).filename, \
                                (*it1).response.substr(u, (*it1).response.length() - u));
                                //std::cout << (*it1).response << std::endl;
                                //exit(6);
                            }
                            else
                            {
                                (*it1).data[r] = '\0';
                                (*it1).response +=  std::string((*it1).data, r);
                            }
                        }
                        memset(&(*it1).data, 0, sizeof((*it1).data));
                        if ((*it1).if_header)
                        {
                            /*std::cout << "<<<" << (*it1).response << ">>>" << std::endl;
                            std::cout << "<<<" << (*it1).datastr << ">>>" << std::endl;
                            std::cout << "filename : " << (*it1).filename << std::endl;
                            std::cout << "boundary : " << (*it1).boundary << std::endl;*/
                        }
                        size_t f = (*it1).datastr.find("\r\n\r\n"); 
                        if (f != std::string::npos)
                        {
                            (*it1).findmethod();
                            if ((*it1).method != "POST")
                            {    
                                (*it1).if_header = 1;
                                /*(*it1).file.open((*it1).method.c_str());
                                if (!(*it1).file.is_open())
                                    std::cerr << "Failed to create a file." << std::endl;*/
                            }
                            else
                            {
                                if (!(*it1).getleftdata)
                                {
                                    f += 4;
                                    int d = (*it1).datastr.length() - f;
                                    (*it1).response = (*it1).datastr.substr(f, d);
                                    (*it1).datastr = (*it1).datastr.substr(0,(*it1).datastr.length() - ((*it1).datastr.length() - f));
                                    (*it1).length += d;
                                    (*it1).getleftdata = 1;
                                    std::cout << "Hoi" << std::endl;
                                }
                            }
                        }
                    }
                }
                else if (FD_ISSET((*it1).client_socket, &wr) && (*it1).if_header)
                {
                    std::cout << (*it1).location << std::endl;
                    std::cout << (*it1).method << std::endl;
                    std::string responseBody;
                    if (checkFileExistence((*it1).location))
                        readFromFile((*it1).location, responseBody);
                    else
                        std::cout << "Sadge <" << (*it1).location << ">" << std::endl;
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
                    std::cout << (*it1).client_socket << "is writable" << std::endl;
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