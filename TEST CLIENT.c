#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define BUF_SIZE 1024
void error_handling(char *message);


int main(int argc, char* argv[])
{
    //ip, 포트번호 지정
    argc=3; argv[1]="10.10.20.111"; argv[2]="9105";
    int sock;
    int fsize=0;
    char message[BUF_SIZE];
    FILE * file;
    struct sockaddr_in serv_addr;
    
    pthread_t t_id;

    if(argc!=3)
    {
        printf("Usage: %s <IP> <PORT>\n",argv[0]);
        exit(1);
    }
    
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");

    file=fopen("Seoul_2013.csv","rt");
    // filesz=sizeof(file);
    while(1)
    {
        fgets(message, BUF_SIZE, file);
        if(feof(file)) break;
        // void* arg[2]={sock,message};
        write(sock, message, strlen(message));
        read(sock, message, BUF_SIZE);
        memset(message, 0, sizeof(message));
    }
    memset(message, 0, sizeof(message));
    read(sock, message, BUF_SIZE);
    printf("%s", message);
    if()
    // file=fopen("Seoul_2014.csv","rt");
    // // filesz=sizeof(file);
    // while(1)
    // {
    //     fgets(message, BUF_SIZE, file);
    //     if(feof(file)) break;
    //     // void* arg[2]={sock,message};
    //     write(sock, message, BUF_SIZE);
    //     read(sock, message, BUF_SIZE);
    //     memset(message, 0, sizeof(message));
    // }
    // if()
    
    // close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}