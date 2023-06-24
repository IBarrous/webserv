#include "Client.hpp"

Client::Client()
{
}
Client::Client(int sock)
{
    client_socket = sock;
}

Client::~Client()
{

}