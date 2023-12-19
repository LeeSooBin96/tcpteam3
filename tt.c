#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFSIZE 1024
// #define PORT 10000
#define LINESIZE 256
int clnt_socket;
void sendFile(const char *filename, int socket);

int main(int argc, char* argv[])
{
    
    struct sockaddr_in serv_addr;

    if(argc!=3)
    {
        printf("Usage: %s <IP><PORT>\n",argv[0]);
        exit(1);
    }
    if((clnt_socket=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(clnt_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1)
    {
        perror("연결 실패");
        exit(EXIT_FAILURE);
    }
    printf("서버와 연결 중\n");

    sendFile("2013_01.csv",clnt_socket);

    close(clnt_socket);
    return 0;
}

void sendFile(const char *filename, int socket)
{
    FILE * file =fopen(filename,"r");
    if(file == NULL)
    {
        perror("파일 연결 실패");
        exit(EXIT_FAILURE);
    }
    char line[LINESIZE];
    char *token;
    int count = 0;

    while(fgets(line, sizeof(line), file) !=NULL)
    {
        token = strtok(line, ",");
        while(token != NULL)
        {
            count++;
            {
                if(count==1||count==2||count==3||count==4||count==5||count==6||count==7)
                send(clnt_socket, token, strlen(token), 0);
                send(clnt_socket,",",1,0);
            }
            token=strtok(NULL, ",");
        }
        // send(clnt_socket, "\n",1,0);
        count=0;
    }
    fclose(file);
}