
/****************************************************************************
* Eyal Levi ID.203249073
* Assignment number 3
* TCP Sockets and Congestion Control
* Ariel University 2020
****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define SERVER_PORT 5066
#define SIZE 2000
#define SERVER_IP_ADDRESS "127.0.0.1"
char buffer[SIZE];
socklen_t length;

void send_file(FILE *fp, int sockfd);
void send_5_times(char* op);



//************************************************** MAIN *************************************************
int main()
{
    send_5_times("cubic");
    send_5_times("reno");
}
//*********************************************************************************************************



//***************************************** SEND FILE  5 TIMES ********************************************
void send_5_times(char* op){
    /* Sending the file 5 times: */
    for (int i = 0; i < 5; i++)
    {
    //==================================================================
        /* Opening TCP socket: */
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
        {
            fprintf(stderr, "Couldn't create the socket : %s\n", strerror(errno));
            exit(EXIT_FAILURE); // failing exit status.
        }

    //==================================================================
        /* Setting the CC algorithm: */
        strcpy(buffer, op);
        length = sizeof(buffer);
        int set_sock_opt = setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buffer, length);
        printf("CC algorithm: %s\n", buffer);
        if(set_sock_opt !=0 )
        {
            perror("setsockopt");
            exit(EXIT_FAILURE); // failing exit status.
        }
        printf("\n================================== \n");

    //==================================================================
        /* Creating connection with Measure.c */
        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(SERVER_PORT);
        int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &server_address.sin_addr);
        if(rval <= 0) {
            printf("inet_pton() failed");
            exit(1);
        }
    //==================================================================
        // Make a connection to the server with socket SendingSocket.
        int connection = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
        if(connection == -1) {
            fprintf(stderr, "connect() failed with error code --> %s\n", strerror(errno));
            exit(EXIT_FAILURE); // failing exit status.
        } 
        else {
            printf("connected to server!\n");
        }

    //==================================================================
        /* Sending the file 5 times: */
        FILE *fp = fopen("1mb.txt", "r");
        if (fp == NULL)
        {
            perror("Error in reading file.");
            exit(1);
        }

        send_file(fp, sock);

        sleep(1);
        close(sock);
    }
}
//*******************************************************************************************************



//*********************************************** SEND FILE **********************************************
void send_file(FILE *fp, int sockfd)
{
    int data_stream;
    int size = 0;
    while( ( data_stream = fread(buffer, 1, sizeof(buffer), fp) ) > 0 )
    {
        size += send(sockfd, buffer, data_stream, 0);
    }
    printf("Bytes sent: %d\n", size);
}
//*********************************************************************************************************

