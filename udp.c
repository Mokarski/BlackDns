#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>


#define MAXRECORDS 1000
#define NameLen 100
#define PORT 53

//
struct DnsName {
char Name[NameLen]; //DNS name req
int Accept; // white DNS name
int Block; // The reason of Block 1-Blocked by system 2- blocked By Roskomnadzor 3-from DNS Blacklists sites 4-Blocked by user or admin
/*
int  SystemBlock;
int  UserBlock;
int  DnsBL;
int  Roskom;
*/
};
struct DnsName DNS[MAXRECORDS]; //Memory cache for records


void write_tocache_dns(char Name[NameLen]){
int c=0;
    for (c=0; c < MAXRECORDS; c++){
          if ( strlen(DNS[c].Name) < 3)   { //empty cell
               if (strlen (Name) < NameLen) strcpy(DNS[c].Name,Name);
               printf("writed req:[%s] \n\r",Name);              
               break; //exit
             }
        }
}


int DNS_Search(char Name[NameLen] ){
int c=0;
int res=0;
    for (c=0; c < MAXRECORDS; c++){
          if ( strstr(DNS[c].Name, Name) == NULL){
              //printf("Not found req \n\r");              
              //if not found dns name then res = 0
             } else {
                       printf ("#%i Req founded: Name[%s] Accept[%i] Block[%i] \n\r",c,DNS[c].Name, DNS[c].Accept, DNS[c].Block);
                       res++; //if founded dns name res = numbers of found
                      }
        }

return res;
}



int main()
{
    setlocale(LC_ALL,"");
    int sockfd; /* Дескриптор сокета */
    int clilen, n; /* Переменные для различных длин 
                                                   и количества символов */
    char line[NameLen]; /* Массив для принятой и 
                                              отсылаемой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры 
                                                      для адресов сервера и клиента */
                                                      
    char DNSReqName[NameLen];
    
    /* Заполняем структуру для адреса сервера: семейство
    протоколов TCP/IP, сетевой интерфейс – любой, номер порта 
    51000. Поскольку в структуре содержится дополнительное не
    нужное нам поле, которое должно быть нулевым, перед 
    заполнением обнуляем ее всю */
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Создаем UDP сокет */
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror(NULL); /* Печатаем сообщение об ошибке */
        exit(1);
    }
    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &servaddr, 
    sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    while(1) {
        /* Основной цикл обслуживания*/
        /* В переменную clilen заносим максимальную длину
        для ожидаемого адреса клиента */
        clilen = sizeof(cliaddr);
        /* Ожидаем прихода запроса от клиента и читаем его. 
        Максимальная допустимая длина датаграммы – 999 
        символов, адрес отправителя помещаем в структуру 
        cliaddr, его реальная длина будет занесена в 
        переменную clilen */
        if((n = recvfrom(sockfd, line, 999, 0,(struct sockaddr *) &cliaddr, &clilen)) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        /* Печатаем принятый текст на экране */
        printf("RECIVED:[%s]\n", line);
        int z;
        printf("\n=========================\n");
        for (z=11; z < NameLen; z++){
                if ( line[z] != 0 ) { 
                   DNSReqName[z] = line[z];
                   printf("%c",DNSReqName[z]);
                   } else break;
            }
        printf("%s\n", DNSReqName);
        if ( DNS_Search(DNSReqName) == 0 ) write_tocache_dns(DNSReqName);
        //printf("%x\n", line);
        /* Принятый текст отправляем обратно по адресу 
        отправителя */
        if(sendto(sockfd, line, strlen(line), 0,(struct sockaddr *) &cliaddr, clilen) < 0){
            perror(NULL);
            close(sockfd);
            exit(1);
        } /* Уходим ожидать новую датаграмму*/
    }
    return 0;
}
