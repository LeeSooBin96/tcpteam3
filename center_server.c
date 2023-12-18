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
    //포트 번호 지정
    argc=2; argv[1]="9106";
    int serv_sock, clnt_sock, p_clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr, p_clnt_adr;
    int clnt_adr_sz,p_clnt_adr_sz, i;

    char message[BUF_SIZE];

    pthread_t t_id;
    if(argc!=2){
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
    if(listen(serv_sock,12)==-1) error_handling("listen() error");

    //각 지역 농산물 센터와 연결
    for(i=0;i<C_CLNT_CNT;i++)
    {
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock; //각 지역 농산물 센터와 연결해주는 소켓 저장
        pthread_mutex_unlock(&mutx);

        //농산물 센터에서 데이터 받아 저장하기
        pthread_create(&t_id,NULL,update_data,(void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected Client %d \n",clnt_cnt); //접속 확인용
    }

    // 클라이언트 통신
    p_clnt_adr_sz=sizeof(p_clnt_adr);
    p_clnt_sock=accept(serv_sock,(struct sockaddr*)&p_clnt_adr,&p_clnt_adr_sz);

    read(p_clnt_sock,message,BUF_SIZE); //클라이언트 요청 수신
    //데이터 한번에 송신!( 상품 가격 리스트, 최소, 최대, 평균, 예상 )
    //년도가 2013,2014이면 내가 가진 데이터에서 검색
    //년도가 2022이면 c클라에 요청


    close(serv_sock);
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
    sprintf(cnt,"data%d.csv",clnt_cnt);
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
