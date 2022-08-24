#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "server.h"
#include "util/hash_table.h"

#define SERVER_UP_TIME 30 // in seconds

hash_table* ht_clients;

double start;
double end;

int socket_fd, client_fd, c, read_size;

void display_summary(){
    printf("\nSUMMARY\n");
    int total = 0;
    for(int i = 0; i < ht_clients->size; i++){
        if(ht_clients->items[i]){
            int num = atoi(ht_clients->items[i]->value);
            printf("%4d transactions from %s\n", num, ht_clients->items[i]->key);
            total = total + num;
        }
    }
    double diff = end - start;
    float rate = total/diff;
    printf("%3.1f transactions/sec  (%d/%1.2lf)\n", rate, total, diff);
}

void timer_callback(int sig){
    display_summary();
    
    // free memory
    free_table(ht_clients);
    
    // close all connections
    if(socket_fd > -1){
        close(socket_fd);
    }

    if(client_fd > -1){
        close(client_fd);
    }

    exit(0);
}

int main(int argc, char *argv[]){
    struct sockaddr_in serv_addr, client_addr;
    char client_message[2000];

    int port;
    struct timeval tv;

    // Get server ip address and port number.
    if(argc == 2){
        port = atoi(argv[1]);
    } else {
        printf("Invalid arguments.\n");
        exit(-1);
    }

    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = timer_callback;
    sigaction(SIGALRM, &sa, NULL);

    alarm(SERVER_UP_TIME);

    // Create socket
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        exit(-1);
    }
    
    // Initialize socket structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Bind
    if(bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind failed");
        exit(-1);
    }
    printf("Using port %d\n", port);

    // Listen
    if(listen(socket_fd, 10) < 0){
        perror("listen failed");
        exit(-1);
    }

    c = sizeof(struct sockaddr_in);

    int num_transactions = 0;
    ht_clients = create_table(HT_CAPACITY);
    bool first = true;
    
    // Start server loop
    while(1){
        // Reset alarm
        alarm(SERVER_UP_TIME);
        // Accept incoming connections
        if((client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t*)&c)) < 0){
            perror("accept failed");
            exit(-1);
        }
        
        // Receive message from client
        while((read_size = recv(client_fd, client_message, 2000, 0)) > 0){
            num_transactions += 1;
            char *msg = client_message;
            char *next_line = strchr(msg, '\n');
            if(next_line) *next_line = '\0';
            
            int n = atoi(msg);

            if(next_line) *next_line = '\n';
            msg = next_line ? (next_line+1) : NULL;

            next_line = strchr(msg, '\n');
            if(next_line) *next_line = '\0';

            char *ht_val = NULL;
            if((ht_val = ht_search(ht_clients, msg))){
                int val = atoi(ht_val);
                val += 1;
                sprintf(ht_val, "%d", val);
                ht_insert(ht_clients, msg, ht_val);
            } else {
                ht_insert(ht_clients, msg, "1");
            }

            // print EPOCH: # count (T   n) from HOSTNAME.PID
            //epoch = time(NULL);
            gettimeofday(&tv, NULL);
            if(first){
                double sec = (double)tv.tv_sec;
                double usec = (double)tv.tv_usec/1000000;
                start = sec + usec;
                first = false;
            }
            printf("%10ld.%.2ld: # %2d (T%3d) from %s\n", tv.tv_sec, (tv.tv_usec/10000), num_transactions, n, msg);
            Trans(n);
            
            // print EPOCH: # count (Done) from HOSTNAME.PID
            //epoch = time(NULL);
            gettimeofday(&tv, NULL);
            double sec = (double)tv.tv_sec;
            double usec = (double)tv.tv_usec/1000000;
            end = sec + usec;
            printf("%10.2lf: # %2d (Done) from %s\n", end, num_transactions, msg);

            int32_t data = htonl(num_transactions);
            write(client_fd, &data, sizeof(data));
        }        
    }
    
    // Program never reaches here.
    display_summary();
    free_table(ht_clients);
    exit(0);
}