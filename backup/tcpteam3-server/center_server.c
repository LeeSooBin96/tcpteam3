#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define C_CLNT_CNT 10 //서울,경기,대구(경북),부산(울산,경남),광주(전남),전주(전북),대전(충남,세종),충북,제주,강원도

void error_handling(char* msg);
void* update_data(void* arg);

int clnt_cnt=0; //연결된 지역 농산물 센터 수
int clnt_socks[C_CLNT_CNT]; //지역 농산물 센터 연결 소자
pthread_mutex_t mutx;

int main(int argc, char* argv[])
{
    char message[BUF_SIZE]="2013,1,112";
    char* year=strtok(message,",");
    char* city=strtok(NULL,",");
    char* item=strtok(NULL,",");
    printf("%s %s %s \n",year,city,item); //년도, 지역코드 , 품목코드

    char cf[20]; sprintf(cf,"data%d.txt",atoi(city)); //지역 분리
    printf("%s\n",cf); //올바른 데이터 파일 열리는가
    FILE* fp=fopen(cf,"r");
    if(fp==NULL){
        puts("파일오픈 실패!");
        return NULL;
    }
    int str_len;
    char list[BUF_SIZE];
    while(1)//수정해야함.
    {
        fgets(list,BUF_SIZE,fp);
        list[strlen(list)]=0;
        // str_len=strlen(list);
        // printf("%s",list);
        if(feof(fp)) break;
        //데이터 걸러서 보내야함.
        if(!strncmp(list,year,4))
        {
            strtok(list,",");strtok(NULL,",");strtok(NULL,","); //조사일, 조사지역명, 품목명 건너뜀
            if(!strncmp(strtok(NULL,","),item,3))
            {
                printf("%s,%s\n",strtok(NULL,","),strtok(NULL,","));
            }
        }
        
        // write(p_clnt_sock,list,BUF_SIZE); 
        // read(p_clnt_sock,list,BUF_SIZE);
        memset(list,0,BUF_SIZE);
    }
    return 0;
}
void error_handling(char* message)
{
fputs(message,stderr);
fputc('\n',stderr);
exit(1);
}
void* update_data(void* arg) //데이터 파일 생성
{
    int clnt_sock=*((int*)arg);
    char msg[BUF_SIZE];
    char cnt[20];

    pthread_mutex_lock(&mutx);
    sprintf(cnt,"data%d.txt",clnt_cnt);
    FILE* fp=fopen(cnt,"wt"); //데이터 파일 오픈
    if(fp==NULL){
        puts("파일오픈 실패!");
        return NULL;
    }
    pthread_mutex_unlock(&mutx);

    while(read(clnt_sock,msg,sizeof(msg))>0) //같은 컴퓨터에서 읽어올때는 안깨지는데...ㅠㅠ
    {
        // msg[strlen(msg)]=0;
        // printf("%s",msg);
        fputs(msg,fp);
        memset(msg,0,BUF_SIZE);
    } 
    fclose(fp);
    return NULL;
}
