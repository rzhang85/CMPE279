// Server side C/C++ program to demonstrate Socket programming, Ru Zhang's submission
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 80

int main(int argc, char const *argv[])
{

    if(argc == 1){  // the default argv would be './server' when you run from command line
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        printf("execve=0x%p\n", execve);

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Attaching socket to port 80
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                      &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        // Forcefully attaching socket to the port 80
        if (bind(server_fd, (struct sockaddr *)&address,
                                     sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // use fork() and setuid() for priviledge seperation
        pid_t pid = fork();
        if(pid < 0){
            printf("Fork failed.\n");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){ // child process
            printf("Child process begin:\n");
            // "nobody" user in Linux is 65534
            if(setuid(65534) == 0){ //On success, zero is returned.  On error, -1 is returned
                printf("Priviledge drop success.\n");
                // actual process start here
                // re-exec to build more secure environment
                printf("Re-exec begins here");
                char  *new_arg[] = {argv[0], new_socket, NULL}; // NULL terminated array of char* strings
                execvp(argv[0], new_arg); // (const char *file, char *const argv[])
            }
            else{
                printf("Priviledge drop failed.\n");
                exit(EXIT_FAILURE);
            }


            printf("Child process ends here.\n");
        }
        else if(pid > 0){ // parent process
            wait();
            printf("Parnet process ends here.\n");
        }
    }
    else if(argc > 1){   // after re-exec, the argv contains
        printf("Success re-exec!\n");
        int new_socket = atoi(argv[1]);
        read_and_write(new_socket);
    }

    return 0;
}


void read_and_write(int new_socket){
    int valread;
    char *hello = "Hello from server";
    char buffer[1024] = {0};

    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
}
