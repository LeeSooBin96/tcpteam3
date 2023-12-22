#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
// #include <pthread.h>
#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
    argc=3; argv[1]="10.10.20.98"; argv[2]="9105";          //IP,PORT 번호 할당
    int sock;
    char message[BUF_SIZE];
    FILE * file;
    struct sockaddr_in serv_addr;
    
    if(argc!=3)
    {
        printf("Usage: %s <IP> <PORT>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
    file=fopen("Seoul_2013.csv", "r");
    while(1)
    {
        fgets(message, BUF_SIZE, file);
        if(feof(file)) break;
        write(sock, message, strlen(message));
        read(sock, message, BUF_SIZE);
        memset(message, 0, sizeof(message));
    }
    memset(message, 0, sizeof(message));

    file=fopen("Seoul_2014.csv", "r");
    while(1)
    {
        fgets(message, BUF_SIZE, file);
        if(feof(file)) break;
        write(sock, message, BUF_SIZE);
        read(sock, message, BUF_SIZE);
        memset(message, 0, sizeof(message));
    }
    memset(message, 0, sizeof(message));
    shutdown(sock,SHUT_WR);
    while(read(sock,message,BUF_SIZE)>0); //수신만 받다가 끝나면
    close(sock); //연결 종료

    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
    while(1)
    {
        
        char item[10];
        if(read(sock, item, 10)>0)
        {
        item[strlen(item)]=0;
        FILE* fp=fopen("Seoul_2022.csv","r");
        if(fp==NULL){
                puts("파일오픈 실패!");
                return NULL;
            }
        char list[BUF_SIZE];
        char snd_msg[50];

        char* lname="규격,조사가격\n";
        write(sock,lname,strlen(lname));
        read(sock,message,BUF_SIZE);        //송신 확인메세지
        while(1)
        {
            fgets(list,BUF_SIZE,fp);
            if(feof(fp)) break;
            list[strlen(list)]=',';
            strtok(list,",");strtok(NULL,",");strtok(NULL,","); //조사일, 조사지역명, 품목명 건너뜀
            if(!strncmp(strtok(NULL,","),item,3)) //품목코드
            {
                strcpy(snd_msg,strtok(NULL,","));strcat(snd_msg,",");strcat(snd_msg,strtok(NULL,","));strcat(snd_msg,"\n"); //보낼데이터(규격, 가격,\n)
                //데이터 걸러서 보내야함.
                write(sock,snd_msg,strlen(snd_msg)); 
                read(sock,snd_msg,strlen(snd_msg));
                printf("%s",snd_msg);
                memset(snd_msg,0,50);
            }

        memset(list,0,BUF_SIZE);
        }
        memset(item,0,10);
        shutdown(sock,SHUT_WR);
        while(read(sock,message,BUF_SIZE)>0); //수신만 받다가 끝나면
        close(sock); //연결 종료
        sock=socket(PF_INET, SOCK_STREAM, 0);
        if(sock==-1)
        error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family=AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
        serv_addr.sin_port=htons(atoi(argv[2]));if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");
        }
    }
}
void error_handling(char* message)
{
fputs(message,stderr);
fputc('\n',stderr);
exit(1);
}