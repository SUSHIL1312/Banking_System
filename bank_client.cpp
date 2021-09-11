/*
                Assignment No:03 (Banking system using client-server Socket Programming)
                                [Client program]

                                        [Group 13]
        members:  Shubham................................. 204101054
                  Stuti Priyambda........................ 204101055
                  Subham Das............................ 204101056
                  Sushil Kumar............................ 204101057
*/
/*
        Execution: compile and run in Terminal.(firstly start the server program then client program)
        for client :  g++ bank_client.cpp -o bank_server
                      ./bank_client <localhost/ip_address> <port_no same as server>

*/


/*
        Client side of the Banking system, It will firstly 
        connect to the Bank server on some port_no.
        Then user will make his query from here
        to the bank server and bank server will 
        perform some task
*/

#include<iostream>
using namespace std;
#include <string>
#include<cstring>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include <fcntl.h> 
#include <unistd.h> 
#include<string.h>
#include<fstream>
#include <arpa/inet.h>
#include<sstream>
#include <netinet/in.h>
#include <netdb.h> 

#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
/*
    receiveMsgFromServer() will receive all msges from the server
	with the help of read()
*/
string receiveMsgFromServer(int sock_fd) {
    int numPacketsToReceive = 0;
    int n = read(sock_fd, &numPacketsToReceive, sizeof(int));
    if(n <= 0) {
        shutdown(sock_fd, SHUT_WR);
        return NULL;
    }
    char *str = (char*)malloc(numPacketsToReceive*RESPONSE_BYTES);
    memset(str, 0, numPacketsToReceive*RESPONSE_BYTES);
    char *str_p = str;
    int i;
    for(i = 0; i < numPacketsToReceive; ++i) {
        int n = read(sock_fd, str, RESPONSE_BYTES);
        str = str+RESPONSE_BYTES;
    }
    return string(str_p);
}
/*sendMsgToServer() will send all the msg to the server over the network 
	using the write().*/

void sendMsgToServer(int sock_fd, string msg) {
     char str[10024];
    strcpy(str,msg.c_str());
    int numPacketsToSend = (strlen(str)-1)/REQUEST_BYTES + 1;
    int n = write(sock_fd, &numPacketsToSend, sizeof(int));
    char *msgToSend = (char*)malloc(numPacketsToSend*REQUEST_BYTES);
    strcpy(msgToSend, str);
    int i;
    for(i = 0; i < numPacketsToSend; ++i) {
        int n = write(sock_fd, msgToSend, REQUEST_BYTES);
        msgToSend += REQUEST_BYTES;
    }
}
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1000];
    if (argc < 3) {
       //fprintf(stderr,"usage %s hostname port\n", argv[0]);
       cout<<"kindly provide both ip_address & portno"<<endl;
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        
        cout<<"Error, no such host"<<endl;
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    else 
        cout<<"Connection Established:"<<endl;
    //connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
    string msgFromServer="";
    string msgToServer="";
    
    while(1)
    {
        msgFromServer=receiveMsgFromServer(sockfd);
        if(msgFromServer.size()==0)
        {
            
            break;
        }
        if(msgFromServer.substr(0,6)=="unauth"){
            
            cout<<"Unauthorised user"<<endl;
            shutdown(sockfd,SHUT_WR);
            break;
        }
          if(msgFromServer.substr(0,13)=="Thanks!! Have"){
          
            cout<<msgFromServer<<endl;
            shutdown(sockfd,SHUT_WR);
            break;
        }
        cout<<msgFromServer<<endl;
        msgFromServer.clear();
        msgToServer.clear();
        getline(cin,msgToServer);
        sendMsgToServer(sockfd,msgToServer);
        if(msgToServer.substr(0,4)=="exit"){
            shutdown(sockfd,SHUT_WR);
            break;
        }
    }
   
    cout<<"Write end closed by the server.\n";
    shutdown(sockfd,SHUT_RD);
    cout<<"Connection closed."<<endl;
    close(sockfd);
    return 0;
}

