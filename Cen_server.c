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

    char msg[BUF_SIZE];
    // char ctn[20];
    // sprintf(ctn,"data%d.txt",cnum);
    FILE* fp=fopen("data1.txt","r"); //데이터 파일 열기
    while(1)
    {
    fgets(msg,BUF_SIZE,fp);
    if(feof(fp)) break;
    read(p_clnt_sock,msg,BUF_SIZE);
    printf("%s",msg);
    // strcpy(msg,"2013,111,1234");
    write(p_clnt_sock,msg,BUF_SIZE); //수신확인메시지
    memset(msg,0,BUF_SIZE);

    }
    
    // pthread_create(&pt_id,NULL,request,(void*)&p_clnt_sock); //클라이언트 요청 처리 --왜 스레드로 나눴지?
    // pthread_detach(pt_id);

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
void* request(void* arg)
{
    int clnt=*((int*)arg);
    char msg[BUF_SIZE];
    read(clnt,msg,BUF_SIZE);
    printf("%s",msg);
    write(clnt,msg,BUF_SIZE); //수신확인메시지
    char* year=strtok(msg,",");
    char* city=strtok(NULL,",");
    char* item=strtok(NULL,",");
    printf("pclient: %s %s %s \n",year,city,item);
    // memset(msg,0,BUF_SIZE); //각 값 저장했으니 메시지 배열 초기화

    // City cnum; //도시별 번호
    // if(!strcmp(city,"서울특별시")) cnum=S;
    // else if(!strcmp(city,"경기도")) cnum=GG;
    // else if(!strcmp(city,"경상북도")) cnum=GB;
    // else if(!strcmp(city,"경상남도")) cnum=GN;
    // else if(!strcmp(city,"전라남도")) cnum=JN;
    // else if(!strcmp(city,"전라북도")) cnum=JB;
    // else if(!strcmp(city,"충청남도")) cnum=CN;
    // else if(!strcmp(city,"충청북도")) cnum=CB;
    // else if(!strcmp(city,"제주특별자치도")) cnum=JJ;
    // else if(!strcmp(city,"강원도")) cnum=GW;
    // else 
    // {
    //     puts("없는 도시명이 입력되었습니다!"); return;
    // }
    
    // //파일명 지정
    // char ctn[20];
    // sprintf(ctn,"data%d.txt",cnum);
    // FILE* fp=fopen(ctn,"r"); //데이터 파일 열기

    // char row[BUF_SIZE];
    // switch(atoi(year)) //년도로 한번 나눔
    // {
    //     case 2013:
    //         printf("2013년이다 \n");
    //         //내가 가진 파일에서 찾아야함
    //         fgets(row,BUF_SIZE,fp); //한줄 읽어옴
    //         fgets(row,BUF_SIZE,fp); //한줄 읽어옴(여기서부터 비교 시작)
    //         while(!strncmp(row,"2013",4)) //2013이 있는동안
    //         {
    //             strtok(row,",");strtok(NULL,","); //조사일 조사지역 건너뜀
    //             if(!strcmp(strtok(NULL,","),item)) //품목명이 같으면
    //             {
    //                 strcat(msg,strtok(NULL,","));
    //                 strcat(msg,",");
    //                 strcat(msg,strtok(NULL,","));
    //             }
    //             memset(row,0,BUF_SIZE); //row 배열 초기화
    //             write(clnt,msg,BUF_SIZE); //파이썬 클라이언트에 전송
    //             read(clnt,msg,BUF_SIZE);
    //             memset(msg,0,BUF_SIZE); //msg 배열 초기화
    //             fgets(row,BUF_SIZE,fp); //다음줄 읽어옴
    //         }
    //         break;
    //     case 2014:
    //         printf("2014년이다 \n");
    //         //내가 가진 파일에서 찾아야함
    //         break;
    //     case 2022:
    //         printf("2022년이다 \n");
    //         //c클라에 요청해야함
    //         break;
    // }
    // fclose(fp);
}