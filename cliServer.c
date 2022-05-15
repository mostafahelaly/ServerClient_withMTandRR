#include <unistd.h>     // / header for unix specic functions declarations : fork(), getpid(), getppid(), sleep()
#include <stdio.h>      // header for input and output from console : printf, perror
#include <stdlib.h>     // header for general functions declarations: exit() and also has exit macros such as EXIT_FAILURE - unsuccessful execution of a program
#include <sys/socket.h> // header for socket specific functions and macros declarations
#include <netinet/in.h> //header for MACROS and structures related to addresses "sockaddr_in", INADDR_ANY
#include <string.h>     // header for string functions declarations: strlen()
#include <sys/types.h>  //for pid , fd
#include <sys/wait.h>   // for wait()
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

#define PORT 5564 // or 5554 tcp client server port
#define WAIT_Q_SIZE 5

struct process
{
    int burst;
    int socket;
    char *name;
    int round;
};

struct process processes[WAIT_Q_SIZE];
int counter = 0;
int numOfProcesses = 5;

void serverExitHandler(int sig_num)
{
    printf("\n Exiting server.  \n");
    fflush(stdout); // force to flush any data in buffers to the file descriptor of standard output,, a pretty convinent function
    exit(0);
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void removeSpecial(char *str)
{
    int i, j;
    for (i = 0; str[i] != '\0'; ++i)
    {
        while (!(str[i] >= 'a' && str[i] <= 'z') && !(str[i] >= 'A' && str[i] <= 'Z') && !(str[i] >= '0' && str[i] <= '9') && !(str[i] == '\0') && !(str[i] == ' ') && !(str[i] == '|') && !(str[i] == '-') && !(str[i] == '&') && !(str[i] == '.') && !(str[i] == '/'))
        {
            for (j = i; str[j] != '\0'; ++j)
            {
                str[j] = str[j + 1];
            }
            str[j] = '\0';
        }
    }
}

void trimSpaces(char *str)
{
    int i;
    int start = 0;
    int end = strlen(str) - 1;

    // find end of leading spaces
    while (isspace((unsigned char)str[start]))
    {
        start++;
    }

    // find the start of the trailing spaces

    while ((end >= start) && isspace((unsigned char)str[end]))
    {
        end--;
    }

    // shift characters
    for (i = start; i <= end; i++)
    {
        str[i - start] = str[i];
    }

    // add null to end of string
    str[i - start] = '\0';
}

void parse_spaces(char *line, char **args)
{
    // remove leading and trailing spaces
    trimSpaces(line);

    // remove special characters
    removeSpecial(line);

    int i = 0;
    // Separate pipes and store strings btn pipes into args
    while ((args[i] = strsep(&line, " ")) != NULL)
    {
        i++;
    }
}

void parse_pipes(char *line, char **args)
{
    // check for pipes and separate commands based on pipes
    int j = 0;
    while ((args[j] = strsep(&line, "|")) != NULL)
    {
        j++;
    }
}

void execute(char **args, int sock)
{
    char *fileCommands[8] = {"touch", "rm", "mkdir", "rmdir", "mv", "tee", "cp", "cd"};
    // These 6 commands do not return anything, so they must be treated with a special case
    // Create a new child to execute the command, then create a second child to send a success message to the client
    if (strcmp(args[0], fileCommands[0]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }
    if (strcmp(args[0], fileCommands[1]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }
    if (strcmp(args[0], fileCommands[2]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }
    if (strcmp(args[0], fileCommands[3]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }
    if (strcmp(args[0], fileCommands[4]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }
    if (strcmp(args[0], fileCommands[5]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }

    if (strcmp(args[0], fileCommands[6]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (execvp(args[0], args) < 0)
            {
                perror("Failed to execute command");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }

    if (strcmp(args[0], fileCommands[7]) == 0)
    {
        pid_t child1, child2;
        child1 = fork();
        if (child1 < 0)
        {
            perror("Failed to create child1");
            exit(EXIT_FAILURE);
        }
        else if (child1 == 0)
        {
            // child1 context
            if (args[1] == NULL)
            {
                perror("cd expected arguments");
            }

            else if (chdir(args[1]) < 0)
            {
                perror("Failed to execute command cd");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // parent context
            waitpid(child1, NULL, 0);
            child2 = fork();
            if (child2 < 0)
            {
                perror("Failed to create child2");
                exit(EXIT_FAILURE);
            }
            else if (child2 == 0)
            {
                // child2 context
                char message[4096] = {0};
                int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                send(sock, message, strlen(message), 0);                            // send message to client
            }
            else
            {
                // parent context
                waitpid(child2, NULL, 0);
            }
        }
        return;
    }

    // printf("No file manipulation commands\n");
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Failed to create Child");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // In child process, execute command using execvp
        dup2(sock, STDOUT_FILENO);
        dup2(sock, STDERR_FILENO);
        close(sock);
        if (execvp(args[0], args) < 0)
        {
            perror("Failed to execute command");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Parent process
        waitpid(pid, NULL, 0);
    }
}

void processOnePipe(char **firstcommand, char **secondcommand, int sock)
{
    int fd1[2]; // pipe
    int pid, pid2;

    // failed to create pipe
    if (pipe(fd1) < 0)
    {
        perror("Failed to create pipe");
        exit(EXIT_FAILURE);
    }

    // create first child process to execute first command
    pid = fork();
    if (pid < 0)
    {
        perror("Failed to create child 1");
        exit(EXIT_FAILURE);
    }

    else if (pid == 0)
    {
        dup2(fd1[1], 1); // write by redirecting standard output to pipe
        close(fd1[0]);   // close the reading part of pipe1
        // write results of execvp onto pipe1
        if (execvp(firstcommand[0], firstcommand) < 0)
        {
            perror("Execvp failed while executing child1");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // wait for first child to execute first command
        waitpid(pid, NULL, 0);

        // create second child process to execute second command
        pid2 = fork();

        if (pid2 < 0)
        {
            perror("Failed to create child to execute second command");
            exit(EXIT_FAILURE);
        }

        // execute second command if second child creation was successfull
        if (pid2 == 0)
        {
            dup2(fd1[0], 0);
            close(fd1[1]);
            close(fd1[0]);

            dup2(sock, STDOUT_FILENO);
            dup2(sock, STDERR_FILENO);
            close(sock);

            if (execvp(secondcommand[0], secondcommand) < 0)
            {
                perror("Execvp failed while executing command 2");
                exit(EXIT_FAILURE);
            }
        }

        else
        {
            // close pipe so that writing is done on standard output
            close(fd1[1]);
            close(fd1[0]);
            waitpid(pid2, NULL, 0);
        }
    }
}

void processTwoPipes(char **firstcommand, char **secondcommand, char **thirdcommand, int sock)
{

    int fd1[2];
    int fd2[2];

    if (pipe(fd1) < 0)
    {
        perror("Failed to create pipe1");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd2) < 0)
    {
        perror("Failed to create pipe2");
        exit(EXIT_FAILURE);
    }

    int pid1;
    int pid2;
    int pid3;

    pid1 = fork();

    if (pid1 < 0)
    {
        perror("Failed to create child 1");
        exit(EXIT_FAILURE);
    }

    else if (pid1 == 0)
    {
        // child 1
        close(1);    // close reading from standard output
        dup(fd1[1]); // redirect standard output to pipe 1

        // close reading and writing end of pipe 2
        close(fd2[0]);
        close(fd2[1]);

        // close reading end of pipe 1
        close(fd1[0]);

        execvp(firstcommand[0], firstcommand); // write results of execvp onto pipe1
        perror("Execvp failed while executing command 1");
        exit(EXIT_FAILURE);
    }

    else
    {
        // in parent process
        // close writing end of pipe 1
        close(fd1[1]);
        waitpid(pid1, NULL, 0);

        pid2 = fork();

        if (pid2 < 0)
        {
            perror("Failed to create child 2");
            exit(EXIT_FAILURE);
        }

        else if (pid2 == 0)
        {
            close(0);    // close standard input
            dup(fd1[0]); // redirect standard input to be pipe 1

            close(1);    // close standard output
            dup(fd2[1]); // redirect standard output to pipe2

            // close writing end of pipe 1
            close(fd1[1]);
            // close reading end of pipe2
            close(fd2[0]);

            execvp(secondcommand[0], secondcommand);
            perror("Execvp failed while executing command 2");
            exit(EXIT_FAILURE);
        }

        else
        {
            // in child 2 parent
            close(fd1[0]); // close reading end of pipe1
            close(fd2[1]); // close writing end of pipe2

            waitpid(pid2, NULL, 0);

            pid3 = fork();

            if (pid3 < 0)
            {
                perror("Failed to create child 3");
                exit(EXIT_FAILURE);
            }

            else if (pid3 == 0)
            {
                close(0); // close standard input
                dup(fd2[0]);

                dup2(sock, STDOUT_FILENO);
                dup2(sock, STDERR_FILENO);
                close(sock);

                // close reading and writing end of pipe1
                close(fd1[0]);
                close(fd1[1]);

                // close writing end of pipe2
                close(fd2[1]);

                execvp(thirdcommand[0], thirdcommand);
                perror("Execvp failed while executing command 3");
                exit(EXIT_FAILURE);
            }

            else
            {
                // in child 3 parent
                close(fd2[0]); // close reading end of pipe2
                waitpid(pid3, NULL, 0);
            }
        }

        //  child1 parent
    }
}

void processThreePipes(char **firstcommand, char **secondcommand, char **thirdcommand, char **fourthcommand, int sock)
{

    // declare and initialize 3 pipes
    int fd1[2];
    int fd2[2];
    int fd3[2];

    if (pipe(fd1) < 0)
    {
        perror("Failed to create pipe1");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd2) < 0)
    {
        perror("Failed to create pipe2");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd3) < 0)
    {
        perror("Failed to create pipe3");
        exit(EXIT_FAILURE);
    }

    // declare 4 pids for 4 child processes to execute 4 commands
    int pid1;
    int pid2;
    int pid3;
    int pid4;

    // create first child process for first command
    pid1 = fork();

    if (pid1 < 0)
    {
        perror("Failed to create child 1");
        exit(EXIT_FAILURE);
    }

    else if (pid1 == 0)
    {
        // in child 1
        close(1);    // close standard output
        dup(fd1[1]); // redirect standard output to pipe1

        // close reading and writing end of pipe3
        close(fd3[0]);
        close(fd3[1]);

        // close reading and writing end of pipe2
        close(fd2[0]);
        close(fd2[1]);

        // close reading end of pipe 1
        close(fd1[0]);

        execvp(firstcommand[0], firstcommand); // write results of execvp onto pipe1
        perror("Execvp failed while executing command 1");
        exit(EXIT_FAILURE);
    }

    else
    {
        // in parent of child 1

        close(fd1[1]); // close writing end of pipe1
        waitpid(pid1, NULL, 0);

        // create child 2 to handle command 2
        pid2 = fork();

        if (pid2 < 0)
        {
            perror("Failed to create child 2");
            exit(EXIT_FAILURE);
        }

        else if (pid2 == 0)
        {
            // in child 2

            close(0);    // close standard input
            dup(fd1[0]); // redirect standard input to pipe1

            close(1);    // close standard output
            dup(fd2[1]); // redirect standard ouput to pipe2

            // close reading and writing end of pipe3
            close(fd3[0]);
            close(fd3[1]);

            // close reading of pipe 2
            close(fd2[0]);

            // close writing of pipe1
            close(fd1[1]);

            execvp(secondcommand[0], secondcommand);
            perror("Execvp failed while executing command 2");
            exit(EXIT_FAILURE);
        }

        else
        {
            // in parent of child 2

            close(fd1[0]); // close reading of pipe1
            close(fd2[1]); // close writing of pipe2
            waitpid(pid2, NULL, 0);

            // create 3rd child process to handle third command

            pid3 = fork();

            if (pid3 < 0)
            {
                perror("Failed to create child 3");
                exit(EXIT_FAILURE);
            }

            else if (pid3 == 0)
            {
                // in child 3
                close(0);    // close standard input
                dup(fd2[0]); // redirect standard input to pipe 2

                close(1);    // close standard output
                dup(fd3[1]); // redirect standard output to pipe3

                // close writing and reading end of pipe1
                close(fd1[0]);
                close(fd1[1]);

                close(fd2[1]); // close writing end of pipe2

                close(fd3[0]); // close reading end of pipe3

                execvp(thirdcommand[0], thirdcommand);
                perror("Execvp failed while executing command 3");
                exit(EXIT_FAILURE);
            }

            else
            {
                // in parent of child 3
                close(fd2[0]); // close reading end of pipe 2
                close(fd3[1]); // close writing end  of pipe3

                waitpid(pid3, NULL, 0);

                // create 4th child process to handle 4th command
                pid4 = fork();

                if (pid4 < 0)
                {
                    perror("Failed to create child 4");
                    exit(EXIT_FAILURE);
                }

                else if (pid4 == 0)
                {
                    // in child 4

                    close(0); // close standard input
                    dup(fd3[0]);

                    dup2(sock, STDOUT_FILENO);
                    dup2(sock, STDERR_FILENO);
                    close(sock);

                    // close reading and writing end of pipe1
                    close(fd1[0]);
                    close(fd1[1]);

                    // close reading and writing end of pipe2
                    close(fd2[0]);
                    close(fd2[1]);

                    close(fd3[1]); // close writing end of pipe3

                    execvp(fourthcommand[0], fourthcommand);
                    perror("Execvp failed while executing command 4");
                    exit(EXIT_FAILURE);
                }

                else
                {
                    // in parent of child 4

                    close(fd3[0]); // close reading end of pipe 3
                    waitpid(pid4, NULL, 0);
                }
            }
        }
    }

    // common ground to child 1 and parent
}

void *HandleClient(void *new_socket)
{
    pthread_detach(pthread_self()); // detach the thread as we don't need to synchronize/join with the other client threads, their execution/code flow does not depend on our termination/completion
    int socket = *(int *)new_socket;
    free(new_socket);

    char message[1024] = {0};  // to send message
    char message2[1024] = {0}; // to receive message
    printf("handling new client in a thread using socket: %d\n", socket);
    while (1)
    {
        // clearing the message arrays properly
        memset(message, 0, 1024 * (sizeof(char)));
        memset(message2, 0, 1024 * (sizeof(char)));

        char **Separatecommands = malloc(64 * sizeof(char *));
        char **first = malloc(64 * sizeof(char *));
        char **second = malloc(64 * sizeof(char *));
        char **third = malloc(64 * sizeof(char *));
        char **fourth = malloc(64 * sizeof(char *));
        struct process cmd;
        printf("Listening to client on socket: %d..\n", socket); // while printing make sure to end your strings with \n or \0 to flush the stream, other wise if in anyother concurent process is reading from socket/pipe-end with standard input/output redirection, it will keep on waiting for stream to end.

        recv(socket, message2, sizeof(message2), 0); // receive message from client

        printf("Client on socket %d, command is: %s\n", socket, message2);

        parse_pipes(message2, Separatecommands);

        if (Separatecommands[1] == NULL)
        {
            parse_spaces(Separatecommands[0], first);
            if (strcmp(first[0], "exit") == 0)
            {
                break;
            }
            else if (strcmp(first[0], "program") == 0)
            {
                cmd.burst = atoi(first[1]);
                cmd.socket = socket;

                // name concatenation
                char buf[20];
                snprintf(buf, 20, "program%d", counter);
                cmd.name = buf;
                cmd.round = 0;

                processes[counter] = cmd;
                counter++;
            }
            else
                execute(first, socket);
        }

        else if (Separatecommands[2] == NULL)
        {
            parse_spaces(Separatecommands[0], first);
            parse_spaces(Separatecommands[1], second);

            processOnePipe(first, second, socket);
        }

        // if three commands / two pipes needed
        else if (Separatecommands[3] == NULL)
        {

            parse_spaces(Separatecommands[0], first);
            parse_spaces(Separatecommands[1], second);
            parse_spaces(Separatecommands[2], third);

            processTwoPipes(first, second, third, socket);
        }

        // if four commands / three pipes needed for execution
        else
        {

            parse_spaces(Separatecommands[0], first);
            parse_spaces(Separatecommands[1], second);
            parse_spaces(Separatecommands[2], third);
            parse_spaces(Separatecommands[3], fourth);

            processThreePipes(first, second, third, fourth, socket);
        }
        if (Separatecommands != NULL)
        {
            free(Separatecommands);
        }

        if (first != NULL)
        {
            free(first);
        }

        if (second != NULL)
        {
            free(second);
        }

        if (third != NULL)
        {
            free(third);
        }

        if (fourth != NULL)
        {
            free(fourth);
        }
    }
    close(socket);
    pthread_exit(NULL);
    return 0;
}

void *Scheduler(void *processes)
{

    struct process *waitQ = (struct process *)processes;
    int quant;

    while (1)
    {
        while (counter == 0)
        {
            // wait for a process to be added to the array
        }

        while (counter != 0)
        {
            for (int i = 0; i < WAIT_Q_SIZE; i++)
            {
                if (waitQ[i].round < 2)
                {
                    quant = 4;
                }

                else if (waitQ[i].round < 5)
                {
                    quant = 6;
                }
                else
                    quant = 10;

                if (waitQ[i].burst <= quant && waitQ[i].burst > 0)
                {

                    printf("\n %s process will finish executing this round %d\n", waitQ[i].name, waitQ[i].round);
                    for (int k = waitQ[i].burst; k > 0; k--)
                    {
                        printf("\n %s process has %d remaining burst time\n", waitQ[i].name, waitQ[i].burst);
                        sleep(1);
                        waitQ[i].burst--;
                    }
                    waitQ[i].burst = 0;
                    counter--;
                    printf("\n %s process has finished executing\n", waitQ[i].name);
                    char message[4096] = {0};
                    int j = snprintf(message, 4096, "command executed successfully\n"); // puts string into buffer
                    send(waitQ[i].socket, message, strlen(message), 0);
                }

                if (waitQ[i].burst > quant)
                {
                    printf("\n %s process is executing in round %d with quantum = %d\n", waitQ[i].name, waitQ[i].round, quant);
                    for (int j = 0; j < quant; j++)
                    {
                        printf("\n %s process has %d remaining burst time\n", waitQ[i].name, waitQ[i].burst);
                        waitQ[i].burst--;
                        
                        sleep(1);
                    }
                }
                waitQ[i].round++;
            }
        }
    }
    pthread_exit(NULL);
    return 0;
}

int main() // main function
{
    signal(SIGINT, serverExitHandler);

    int sock1, sock2;
    struct sockaddr_in address; // structure for storing addres; local interface and port
    int addrlen = sizeof(address);

    // Creating socket file descriptor with communication: domain of internet protocol version 4, type of SOCK_STREAM for reliable/conneciton oriented communication, protocol of internet
    if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == 0) // checking if socket creation fail
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // setting the address to be bind to socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // attaching socket to addresses (any/all local ip with port 5564)
    if (bind(sock1, (struct sockaddr *)&address,
             sizeof(address)) < 0) // checking if bind fails
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sock1, 10) < 0) // defining for socket length of queue for pending client connections
    {
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    }

    int sched_rc;
    pthread_t scheduler_id;
    sched_rc = pthread_create(&scheduler_id, NULL, Scheduler, processes);
    if (sched_rc)
    {
        printf("\n ERROR: return code from pthread_create for scheduler is %d \n", sched_rc);
        exit(EXIT_FAILURE);
    }

    while (1) // to keep server alive for infintiy
    {
        if ((sock2 = accept(sock1, (struct sockaddr *)&address,
                            (socklen_t *)&addrlen)) < 0) // accepting the client connection with creation/return of a new socket for the established connection to enable dedicated communication (active communication on a new socket) with the client
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int rc;                                       // return value from pthread_create to check if new thread is created successfukky                           */
        pthread_t thread_id;                          // thread's ID (just an integer, typedef unsigned long int) to indetify new thread
        int *new_socket = (int *)malloc(sizeof(int)); // for passing safely the integer socket to the thread
        if (new_socket == NULL)
        {
            fprintf(stderr, "Couldn't allocate memory for thread new socket argument.\n");
            exit(EXIT_FAILURE);
        }
        *new_socket = sock2;

        // create a new thread that will handle the communication with the newly accepted client
        rc = pthread_create(&thread_id, NULL, HandleClient, new_socket);
        if (rc) // if rc is > 0 imply could not create new thread
        {
            printf("\n ERROR: return code from pthread_create is %d \n", rc);
            exit(EXIT_FAILURE);
        }
    }
    close(sock1);
    return 0;
}