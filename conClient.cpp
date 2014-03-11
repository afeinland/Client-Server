// Course: CS 100 <Winter 2014>
//
// First Name: Alex
// Last Name: Feinland
// CS Login: afein001
// UCR email: afein001@ucr.edu
// SID: 861056485
//
// Lecture Section: <001>
// Lab Section: <021>
// TA: Bryan Marsh 
//
// Assignment <hw8>
//
// I hereby certify that the code in this file 
// is ENTIRELY my own original work.

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <cerrno>
#include "PortNumber.h"

using std::cout;
using std::endl;
using std::cerr;

void error(const char * msg)
{
    cerr << msg << errno << endl;
    exit(-1);
}


/* BEGIN THREAD FUNCTIONS */

struct Thread_data
{
    int id;
    int fd;
    pthread_t thread_id;
    const char * file;
    char * newdir;
};

void make_empty_dir(Thread_data & t_data)
{
    char dirname[BUFSIZ];
    mode_t mode = 0777;

    sprintf(dirname, "Thread%dfiles", t_data.id);
    mkdir(dirname, mode);
    t_data.newdir = dirname;
}



void * download_file_from_server(void * data)
{
    struct Thread_data * t_data = (struct Thread_data *)data;

    make_empty_dir(*t_data);
    //cout << "id: " << t_data->id << " fd: " << t_data->fd << " file: " << t_data->file << endl;
    cout << "newdirname: " << t_data->newdir << endl;



    return NULL;
}
/* END THREAD FUNCTIONS */


/* BEGIN SOCKET INIT FUNCTIONS */
/* END SOCKET INIT FUNCTIONS */


int main(int argc, char * argv[]) // (0)./conClient (1)well.cs.ucr.edu (2)/path/to/file_to_copy (3)number_of_copies
{
    if(argc < 4)
        error("CLIENT: TOO FEW ARGUMENTS");

    const int threadcount = atoi(argv[3]); // number of files passed in = number of threads we will create
    const char * filepath = argv[2];
    int sockFD;
    int port = PORTNUM;
    struct sockaddr_in serverAddr;
    struct hostent * server;
    Thread_data thread_args[threadcount];

    if( !(server = gethostbyname(argv[1])) )
        error("CLIENT: ERROR GETTING HOST NAME");

    if( (sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("CLIENT: SOCKET CREATION FAILURE");
    cout << "Client: Socket Created" << endl;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    memcpy(server->h_addr, &serverAddr.sin_addr.s_addr, server->h_length);
    serverAddr.sin_port = htons(port);

    if( connect(sockFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0 )
        error("CLIENT: ERROR CONNECTING TO SOCKET");

    for(int i = 0; i < threadcount; ++i)
    {
        Thread_data & t = thread_args[i];
        t.id = i;
        t.fd = sockFD;
        t.file = filepath;
        pthread_create(&thread_args[i].thread_id, NULL, download_file_from_server, (void *)&t);
    }

    for(int i = 0; i < threadcount; ++i)
    {
        pthread_join(thread_args[i].thread_id, NULL);
    }


    shutdown(sockFD, 2);
    close(sockFD);

    return 0;
}

