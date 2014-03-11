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
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <cerrno>
#include <pthread.h>
#include <signal.h>
#include <cstring>
#include "PortNumber.h"

using std::cout;
using std::endl;
using std::cerr;

const int NUM_THREADS = 3;

void error(const char * msg)
{
    cerr << msg << errno << endl;
    exit(-1);
}

void sighandler(const int signum)
{
    switch(signum)
    {
        case SIGCHLD:
            return;
    }
}


void sendFile(const char * filename)
{
    cout << "file: " << filename << endl;
}

void copyDir(const int clientFD, char * dir)
{
    DIR * dirp;
    struct dirent * myfile;
    struct stat st;
    char buf[BUFSIZ];
    char * dname;

    dirp = opendir(dir);

    cout << "clientFD: " << clientFD << " dir: " << dir << endl;

    if(!dirp) // dir is a file, so send it over
    {
        sendFile(dir);
        return;
    }


    while( (myfile = readdir(dirp)) )
    {
        dname = myfile->d_name;
        lstat(dname, &st);

        if(dname[0] == '.')
            continue;
        
        if(S_ISDIR(st.st_mode))
            cout << "dir: " << dname << endl;
        else
            cout << "file: " << dname << endl;


        memset(buf, '\0', BUFSIZ);
    }

}


void * serveClient(void * cfd)
{
    const int clientFD = *((int *)(&cfd));
    char dirName[256];

    memset(dirName, '\0', sizeof(dirName));
    read(clientFD, dirName, 256);
    cout << "dirName from client: " << dirName << endl;

    copyDir(clientFD, dirName);

    close(clientFD);

    return 0;

}

void clearSock(struct sockaddr_in & a, struct sockaddr_in & b)
{
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
}

void openSock(int & fd)
{
    if( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("SERVER: SOCKET CREATION FAILURE");
    cout << "Server: Socket created" << endl;
}

void setSockOpt(const int & fd)
{
    int optval = 1;
    if( setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1 )
        error("SERVER: ERROR SETTING SOCKET OPTIONS");
}

void setSockAddrInfo(struct sockaddr_in & sa, const int port)
{
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
}

void bindSock(const int fd, struct sockaddr_in sa)
{
    if( bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0 )
        error("SERVER: ERROR BINDING TO SOCKET");
}

void listenSock(const int fd)
{
    if( listen(fd, NUM_THREADS) < 0 )
        error("SERVER: ERROR LISTENING ON PORT");
}

int main() // ./conServer
{
    int sockFD, newsockFD, port = PORTNUM;
    socklen_t clientLen;
    struct sockaddr_in serverAddr, clientAddr;
    pthread_t threads[NUM_THREADS];


    clearSock(serverAddr, clientAddr);

    signal(SIGCHLD, sighandler);

    openSock(sockFD);

    setSockOpt(sockFD);

    setSockAddrInfo(serverAddr, port);

    bindSock(sockFD, serverAddr);

    listenSock(sockFD);

    while(1)
    {
        clientLen = sizeof(clientAddr);

        for(int i = 0; i < NUM_THREADS; ++i)
        {
            if( (newsockFD = accept(sockFD, (struct sockaddr *) &clientAddr, &clientLen)) < 0 )
                error("SERVER: ERROR ACCEPTING CLIENT");
            if( int rc = pthread_create(&threads[i], NULL, serveClient, (void *)newsockFD) )
            {
                cerr << "THREAD ERROR CODE IS: " << rc << endl;
                error("SERVER: ERROR CREATING THREAD");
            }
            
        }

        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);
    }


    return 0;
}

