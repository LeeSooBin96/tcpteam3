#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define C_CLNT_CNT 10 //서울, 경기, 경북(대구), 경남(부산, 울산), 전남(광주), 전북(전주), 충남(대전,세종), 충북(청주), 제주, 강원도

void error_handling(char* msg);
void* update_data(void* arg);
void* request(void* arg);

int clnt_cnt=0; //연결된 지역 농산물 센터 수 저장
int clnt_socks[C_CLNT_CNT]; //지역 농산물 센터 연결 소자
pthread_mutex_t mutx;

typedef enum { S=1, GG, GB, GN, JN, JB, CN, CB, JJ, GW }City; //도시 번호

int main(int argc, char* argv[])
{
   //포트 번호 지정
   argc=2; argv[1]="9106";
   int serv_sock, clnt_sock, p_clnt_sock;
   struct sockaddr_in serv_adr,clnt_adr, p_clnt_adr;
   int clnt_adr_sz, p_clnt_adr_sz, i; 
   
   pthread_t t_id, pt_id;
   if(argc!=2)
   {
        printf("Usage : %s <port> \n",argv[0]);
        exit(1);
   }

   pthread_mutex_init(&mutx,NULL);
   serv_sock=socket(PF_INET,SOCK_STREAM,0);

   memset(&serv_adr,0,sizeof(serv_adr));
   serv_adr.sin_family=AF_INET;
   serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
   serv_adr.sin_port=htons(atoi(argv[1]));

   if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1) error_handling("bind() error");
   if(listen(serv_sock,5)==-1) error_handling("listen() error");

// //    각 지역 농산물 센터와 연결
//    for(i=0;i<C_CLNT_CNT;i++)
//    {
//         clnt_adr_sz=sizeof(clnt_adr);
//         clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

//         pthread_mutex_lock(&mutx);
//         clnt_socks[clnt_cnt++]=clnt_sock; //각 지역 농산물 센터와 연결해주는 소켓 저장
//         pthread_mutex_unlock(&mutx); //안겹치게 임계영역 설정

//         //농산물 센터에서 데이터 받아 저장하기
//         pthread_create(&t_id,NULL,update_data,(void*)&clnt_sock);
//         pthread_detach(t_id);
//         printf("Connected Client %d \n",clnt_cnt); // 접속 확인용
//    }

    // 클라이언트 통신
    p_clnt_adr_sz=sizeof(p_clnt_adr);
    p_clnt_sock=accept(serv_sock,(struct sockaddr*)&p_clnt_adr,&p_clnt_adr_sz);

    //파이썬 클라이언트 스레드 통신이 안됨...
    char message[BUF_SIZE];
    read(p_clnt_sock,message,BUF_SIZE);
    write(p_clnt_sock,message,strlen(message)); //수신 확인메시지
    printf("%s\n",message); //받은 메시지 확인(년도, 지역, 품목)
    
    char* year=strtok(message,",");
    char* city=strtok(NULL,",");
    char* item=strtok(NULL,",");
    printf("%s %s %s \n",year,city,item); //년도, 지역코드 , 품목코드

    //년도에서 2022이면 c클라이언트에 요청해야함
    // if(atoi(city)==2022)
    // {

    // }
    // else //아니면 내가 처리
    // {   
        char cf[20]; sprintf(cf,"data%d.txt",atoi(city)); //지역 분리
        printf("%s\n",cf);
        FILE* fp=fopen(cf,"r");
        if(fp==NULL){
            puts("파일오픈 실패!");
            return NULL;
        }
        char list[BUF_SIZE];
        char snd_msg[50];
        int str_len;

        char* lname="규격,조사가격\n";
        write(p_clnt_sock,lname,strlen(lname));
        printf("%s \n",lname);
        while(1)//수정해야함.
        {
            fgets(list,BUF_SIZE,fp);
            if(feof(fp)) break;
            // fgets(list,BUF_SIZE,fp);
            list[strlen(list)]=',';
            // str_len=strlen(list);
            // printf("%s",list);
            if(!strncmp(list,year,4)) 
            {
                strtok(list,",");strtok(NULL,",");strtok(NULL,","); //조사일, 조사지역명, 품목명 건너뜀
                if(!strncmp(strtok(NULL,","),item,3)) //품목코드
                {
                    strcpy(snd_msg,strtok(NULL,","));strcat(snd_msg,",");strcat(snd_msg,strtok(NULL,","));strcat(snd_msg,"\n"); //보낼데이터(규격, 가격,\n)
                    //데이터 걸러서 보내야함.
                    write(p_clnt_sock,snd_msg,strlen(snd_msg)); 
                    printf("%s",snd_msg);
                    memset(snd_msg,0,50);
                }
            }

            memset(list,0,BUF_SIZE);
        }
    // }

//    for(i=0;i<C_CLNT_CNT;i++) close(clnt_socks[i]);
    // close(p_clnt_sock);
//    close(serv_sock);
//    return 0;
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
    FILE* fp=fopen(cnt,"w"); //데이터 파일 오픈 및 생성
    if(fp==NULL){
        puts("파일오픈 실패!");
        return NULL;
    }
    pthread_mutex_unlock(&mutx); //여기까지는 문제 없음(전역변수 건들여서 임계영역)

    int str_len;

    while(str_len=read(clnt_sock,msg,BUF_SIZE))//수정해야함.
    {
         //전송받음
        write(clnt_sock,msg,BUF_SIZE); //전송받은것 그대로 전송 에코 써야하나했음...
        fputs(msg,fp);
        memset(msg,0,BUF_SIZE);
    }
    fclose(fp);
}
