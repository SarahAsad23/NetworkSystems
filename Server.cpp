#include <cstdio>
#include <iostream>
#include <sys/types.h>		//socket, bind
#include <sys/socket.h>		//coket, bind, listen, inet_ntoa
#include <netinet/in.h>		//hton1, htons, inet_ntoa
#include <arpa/inet.h>		//intet_ntoa
#include <netdb.h>			//gethostbyname
#include <unistd.h>			//read, write, close 
#include <strings.h>		//bzero
#include <netinet/tcp.h>	//SO_REUSEADDR
#include <sys/uio.h>		//writev
#include <pthread.h>        //pthread
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <fcntl.h>  
#include "Server.h"
#include "CommandType.h"
#include "Client.h"


//sets up the server side socket and listens for incoming connections. 
int Server::Start(int port) {

    //set up addr atructure 
    struct sockaddr_in hints;
    memset(&hints, 0, sizeof(hints));

    hints.sin_family = AF_INET; // IPv4 or v6
    hints.sin_port = htons(port);

    //create socket 
    this->serverSd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSd < 0) {
        cerr << "Error creating socket (server)";
        return -1;
    }

    //lose the "addres already in use" error message
    const int yes = 1;
    setsockopt(this->serverSd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

    //Bind socket 
    bind(this->serverSd, (sockaddr*)&hints, sizeof(hints));

    //listen for connection requests 
    listen(this->serverSd, 5);

    //client socket 

    cout << "Server ready and accepting connections\n";

    //keep accepting connections until forceful quit 
    while (1)
    {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);

        //accept connections
        int clientSd = accept(this->serverSd, (sockaddr*)&client, &clientSize);
        cout << "Connection accepted" << endl;
    }
}

int Server::processRequest(int clientSd)
{
    // Read a single byte so that we can know what kind of command is being sent by the client.
    unsigned char command;
    int err = recv(this->serverSd, &command, 1, 0);
    if (err < 0) {
        return err;
    }
    
    cout << "Enter 1 to get a list of all waiting games\n";
    cout << "Enter 2 to start a new game\n";
    cout << "enter 3 to join an existing game\n";

    cout << "Received Command: " << command << endl;
    
    Client client; 

    switch (command) {
    case CommandType::GetWaitingGames:
        client.getAllWaitingGames();
        break;

    case CommandType::StartNewGame:
        client.startNewGame();
        break;


    case CommandType::JoinExistingGame:
        client.joinExistingGame();
        break;


    default:
        cerr << "Unknown Command received: " << command << endl;
        return -1;
    }
}