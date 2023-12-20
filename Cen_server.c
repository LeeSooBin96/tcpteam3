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
   argc=2; argv[1]="9105";
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
   for(i=0;i<C_CLNT_CNT;i++)
   {
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock; //각 지역 농산물 센터와 연결해주는 소켓 저장
        pthread_mutex_unlock(&mutx); //안겹치게 임계영역 설정

        //농산물 센터에서 데이터 받아 저장하기
        pthread_create(&t_id,NULL,update_data,(void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected Client %d \n",clnt_cnt); // 접속 확인용
   }

    // 클라이언트 통신
    p_clnt_adr_sz=sizeof(p_clnt_adr);
    p_clnt_sock=accept(serv_sock,(struct sockaddr*)&p_clnt_adr,&p_clnt_adr_sz);

    //파이썬 클라이언트 스레드 통신이 안됨...
    char message[BUF_SIZE];
    read(p_clnt_sock,message,BUF_SIZE);
    printf("%s\n",message); //받은 메시지 확인(년도, 지역, 품목)
    write(p_clnt_sock,message,strlen(message)); //수신 확인메시지
    
    int year=atoi(strtok(message,","));
    int city=atoi(strtok(NULL,","));
    int item=atoi(strtok(NULL,","));
    printf("%d %d %d \n",year,city,item); //년도, 지역코드 , 품목코드

    write(clnt_socks[city-1],"hello",10);

    char cf[20]; sprintf(cf,"data%d.txt",city);
    printf("%s\n",cf);
    FILE* fp=fopen(cf,"r");
    if(fp==NULL){
        puts("파일오픈 실패!");
        return NULL;
    }
    char list[BUF_SIZE];
    int str_len;
    while(1)//수정해야함.
    {
        fgets(list,BUF_SIZE,fp); // 파일 데이터 수신에서 오류남...
        str_len=strlen(list);
        printf("%s",list);
        if(feof(fp)) break;
        //데이터 걸러서 보내야함.
        write(p_clnt_sock,list,str_len); 
        memset(list,0,BUF_SIZE);
    }

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
    FILE* fp=fopen(cnt,"a+"); //데이터 파일 오픈 및 생성
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
        msg[str_len]='\n';
        fputs(msg,fp);
        memset(msg,0,BUF_SIZE);
    }
    fclose(fp);
}
