#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

#include "server.h"


void get_input(char *input_buf){
    char buffer[200];

    while(scanf("%s", buffer) != EOF){
        //strcat(input_buf, buffer);
        strcpy(input_buf, buffer);
        input_buf += strlen(buffer);
        *input_buf = '\n';
        input_buf += 1;
        //n += (strlen(buffer) + 1);
    }
    *input_buf = '\0';
}

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in server_addr;
    int server_reply = 0;
    
    struct timeval tv;
    int port;
    char *ip;

    // Get server ip address and port number.
    if(argc == 3){
        port = atoi(argv[1]);
        ip = argv[2];
    } else {
        printf("Invalid arguments.\n");
        exit(-1);
    }

    // Get commands to execute.
    char commands[1000];
    get_input(commands);

    // Create socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("could not create socket.");
        exit(-1);
    }
    
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Connect to server
    if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connection failed");
        exit(-1);
    }
    
    // print HOSTNAME.PID
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);
    pid_t pid = getpid();
    char out_filename[sizeof(hostname) + sizeof(pid) + 1];
    sprintf(out_filename, "%s.%d", hostname, pid);
    
    // open file to write output
    FILE *out_file = fopen(out_filename, "w");
    if(out_file == NULL){
        perror("Could not open output file.");
        exit(-1);
    }

    fprintf(out_file, "Using port %d\n", port);
    fprintf(out_file, "Using server address %s\n", ip);
    fprintf(out_file, "Host %s\n", out_filename);

    // Communicate
    int num_transactions = 0;
    char *p_cmd = commands;
    while(p_cmd){
        char *next_line = strchr(p_cmd, '\n');
        if(next_line) *next_line = '\0';
        
        int n = atoi(p_cmd + 1);
        
        // Send data
        if(*p_cmd == 'T'){
            // print EPOCH: Send (T n)
            // epoch = gettimeofday();
            gettimeofday(&tv, NULL);
            fprintf(out_file, "%10ld.%02ld: Send (T%3d)\n", tv.tv_sec, (tv.tv_usec/10000), n);

            char send_data[HOST_NAME_MAX + 6];
            sprintf(send_data, "%d\n%s.%d\n", n, hostname, (int)getpid());
            
            if(send(sock, send_data, strlen(send_data), 0) < 0){
                perror("Send failed");
                exit(-1);
            }

            num_transactions++;

            // Receive reply
            if(recv(sock, &server_reply, sizeof(server_reply), 0) < 0){
                perror("Receive failed");
                exit(-1);
            }

            server_reply = ntohl(server_reply);

            // print EPOCH: Recv (D n)
            // epoch = time(NULL);
            gettimeofday(&tv, NULL);
            fprintf(out_file, "%10ld.%02ld: Recv (D%3d)\n", tv.tv_sec, (tv.tv_usec/10000), server_reply);

        } else if(*p_cmd == 'S'){
            // print Sleep n units
            fprintf(out_file, "Sleep %d units\n", n);
            Sleep(n);
        }

        // Restore string
        if(next_line) *next_line = '\n';
        p_cmd = next_line ? (next_line+1) : NULL;
    }

    close(sock);
    
    fprintf(out_file, "Sent %d transactions\n", num_transactions);
    
    fclose(out_file);

    exit(0);
}