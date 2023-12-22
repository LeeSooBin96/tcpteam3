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

int clnt_cnt=0; //연결된 지역 농산물 센터 수 저장
int clnt_socks[C_CLNT_CNT]; //지역 농산물 센터 연결 소자
pthread_mutex_t mutx;


int main(int argc, char* argv[])
{
   //포트 번호 지정
   argc=2; argv[1]="9106";
   int serv_sock, clnt_sock, p_clnt_sock;
   struct sockaddr_in serv_adr,clnt_adr, p_clnt_adr;
   int clnt_adr_sz, p_clnt_adr_sz, i; 
   char signal[BUF_SIZE];
   
   pthread_t t_id;
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

// 각 지역 농산물 센터와 연결 후 13, 14년도 데이터 파일 받기
   for(i=0;i<C_CLNT_CNT;i++) 
   {
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

        //농산물 센터에서 데이터 받아 저장하기(스레드로 처리)
        pthread_create(&t_id,NULL,update_data,(void*)&clnt_sock);
        pthread_detach(t_id);
   }

//    각 지역 농산물 센터와 재연결
   for(i=0;i<C_CLNT_CNT;i++) 
   {
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[i]=clnt_sock; //각 지역 농산물 센터와 연결해주는 소켓 저장
        pthread_mutex_unlock(&mutx); //안겹치게 임계영역 설정
        printf("Connected Client... \n",clnt_cnt); // 접속 확인용
   }

    while(1) //p 클라이언트가 종료하지 않는 이상 서버 종료되지 않음.
    {
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
        if(atoi(year)==2022)
        {   
            char buf[BUF_SIZE];
            write(clnt_socks[atoi(city)-1],item,strlen(item)); //메세지 송신
            while(read(clnt_socks[atoi(city)-1],buf,BUF_SIZE)) //데이터 읽어오기
            {
                write(clnt_socks[atoi(city)-1],buf,BUF_SIZE); //수신확인메시지
                write(p_clnt_sock,buf,strlen(buf)); //파이썬클라이언트에게 보내기
                printf("%s",buf);
                memset(buf,0,BUF_SIZE);
            }
            close(clnt_socks[atoi(city)-1]); //연결된 c클라이언트 종료
            clnt_adr_sz=sizeof(clnt_adr);
            clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
            clnt_socks[atoi(city)-1]=clnt_sock;
        }
        else //아니면 내가 처리
        {   
            char cf[20]; sprintf(cf,"data%d.txt",atoi(city)); //지역 분리
            FILE* fp2=fopen(cf,"r");
            if(fp2==NULL){
                puts("파일오픈 실패!");
                return NULL;
            }
            char list[BUF_SIZE];
            char snd_msg[50]; char c_size[50]; char c_price[50];
            int max=0, min=9999999, sum=0, count=0;

            char* lname="규격,조사가격\n";
            write(p_clnt_sock,lname,strlen(lname));
            while(1)//수정해야함.
            {
                fgets(list,BUF_SIZE,fp2);
                if(feof(fp2)) break;
                list[strlen(list)]=',';
                if(!strncmp(list,year,4)) 
                {
                    strtok(list,",");strtok(NULL,",");strtok(NULL,","); //조사일, 조사지역명, 품목명 건너뜀
                    if(!strncmp(strtok(NULL,","),item,3)) //품목코드
                    {
                        int size=atoi(strtok(NULL,",")); int price=atoi(strtok(NULL,",")); //숫자를 문자로ㅗㅗㅗ
                        if(price>max) max=price;
                        if(price<min) min=price;
                        sum+=price;
                        sprintf(c_size,"%d",size);sprintf(c_price,"%d",price);
                        strcpy(snd_msg,c_size);strcat(snd_msg,",");strcat(snd_msg,c_price);strcat(snd_msg,"\n"); //보낼데이터(규격, 가격,\n)
                        //데이터 걸러서 보내야함.
                        write(p_clnt_sock,snd_msg,strlen(snd_msg)); count++;
                        memset(snd_msg,0,50);memset(c_size,0,50);memset(c_price,0,50);
                    }
                }
                memset(list,0,BUF_SIZE);
            }
        }
        memset(message,0,BUF_SIZE);
        shutdown(p_clnt_sock,SHUT_WR);  
        close(p_clnt_sock);
    }
   for(i=0;i<C_CLNT_CNT;i++) close(clnt_socks[i]);
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
    char msg[BUF_SIZE+1];
    char cnt[20];

    read(clnt_sock,msg,3);
    write(clnt_sock,"connecting...\n",15); //수신확인

    sprintf(cnt,"data%s.txt",msg); //지역별 파일에 저장(01:서울 02:경기 03:경북, 04:경남, 05:전남, 06:전북, 07:충남, 08:충북, 09:제주, 10:강원)
    FILE* fp=fopen(cnt,"w"); //데이터 파일 오픈 및 생성
    if(fp==NULL){
        puts("파일오픈 실패!");
        return NULL;
    }

    int str_len;

    while((str_len=read(clnt_sock,msg,BUF_SIZE))>0)//수정해야함.
    {
         //전송받음
        write(clnt_sock,msg,BUF_SIZE); //전송받은것 그대로 전송 에코 써야하나했음...
        fputs(msg,fp);
        memset(msg,0,BUF_SIZE);
    }
    puts("data save fin...");
    fclose(fp);
    close(clnt_sock);
    return;
}
