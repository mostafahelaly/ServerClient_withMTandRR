#include <unistd.h>     // / header for unix specic functions declarations : fork(), getpid(), getppid(), sleep()
#include <stdio.h>      // header for input and output from console : printf, perror
#include <sys/socket.h> // header for socket specific functions and macros declarations
#include <stdlib.h>     // header for general functions declarations: exit() and also has exit macros such as EXIT_FAILURE - unsuccessful execution of a program
#include <netinet/in.h> //header for MACROS and structures related to addresses "sockaddr_in", INADDR_ANY
#include <string.h>     // header for string functions declarations: strlen()
#include <arpa/inet.h>  // header for functions related to addresses from text to binary form, inet_pton
#include <sys/types.h>  //for pid , fd
#include <sys/wait.h>   // for wait()
#include <ctype.h>
#include <signal.h>
#include <fcntl.h> //fcntl
#include <sys/time.h>

#define PORT  5564 
#define CHUNK_SIZE 2048
#define TIMEOUT 4

int recvAll(int socket, int timeout)
{
    int size_recv, total_size = 0; // number of bytes received per chunk and total number of bytes received
    struct timeval begin, now;
    char chunk[CHUNK_SIZE]; // buffer to receive data into
    double timediff;

    fcntl(socket, F_SETFL, O_NONBLOCK); // make socket non-blocking
    gettimeofday(&begin, NULL);         // beginning time

    while (1)
    {
        gettimeofday(&now, NULL);

        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec); // time elapsed in seconds

        if (total_size > 0 && timediff > timeout) // if you got some data, then break after timeout
        {
            break;
        }

        else if (timediff > timeout * 2) // if you got no data at all, wait a little longer, twice the timeout
        {
            break;
        }


        memset(chunk, 0, CHUNK_SIZE);
        if ((size_recv = recv(socket, chunk, CHUNK_SIZE, 0)) <= 0)
            usleep(100000); // wait 0.1 seconds if nothing is received and then try again
        else
        {
            total_size += size_recv;
            printf("%s", chunk);
            gettimeofday(&begin, NULL);
        }
    }
    printf("\n");
    return total_size;
}

int sock = 0;

void clientExitHandler(int sig_num)
{
    send(sock, "exit", strlen("exit"), 0); // sending exit message to server
    close(sock);                           // close the socket/end the conection
    printf("\n Exiting client.  \n");
    fflush(stdout); // force to flush any data in buffers to the file descriptor of standard output,, a pretty convinent function
    exit(0);
}

int main()
{
    signal(SIGINT, clientExitHandler);

    struct sockaddr_in serv_addr; // structure for storing addres; remote server ip and port

    // Creating socket file descriptor with communication: domain of internet protocol version 4, type of SOCK_STREAM for reliable/conneciton oriented communication, protocol of internet
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) // checking if socket creation fail
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    // setting the address to connect socket to server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form and set the ip
    // This function converts the character string 127.0.0.1 into a network
    // address structure in the af address family, then copies the
    // network address structure to serv_addr.sin_addr
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) // check if conversion failed
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // connect the socket with the adddress and establish connnection with the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char *fileCommands[8] = {"touch", "rm", "mkdir", "rmdir", "mv", "tee", "cp", "cd"}; // file manipulation commands

    while (1)
    {
        char **Separatecommands = malloc(64 * sizeof(char *));
        char message[4096] = {0};  // to receive message from server
        char message2[4096] = {0}; // message to send to server
        printf("> ");
        fgets(message2, sizeof(message2), stdin);
        if (strlen(message2) > 1)
        {
            if (strcmp(message2, "exit") == 10)
                break;

            printf("Client message: %s", message2);

            send(sock, message2, strlen(message2), 0); // send message to server
            printf("Command sent to server\n");

            // recv(sock, message, sizeof(message), 0);
            // printf("\n%s\n", message);

            recvAll(sock, TIMEOUT);
        }
    }
    close(sock);
    return 0;
}