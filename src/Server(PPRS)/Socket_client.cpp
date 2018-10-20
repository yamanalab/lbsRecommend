/*
 *This program has 1 task
 *1.recieve public key/secret key file through socket
 * */

//encryption
#include "FHE.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <time.h>

//socket setting
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FILE_NAME_MAX_SIZE  20  
#define BUFFER_SIZE 1024 
using namespace std;
int main(int argc, char **argv){

//socket setting start --------------------------------
int sock = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in serv_addr;
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr("192.168.101.3"); 
serv_addr.sin_port = htons(1234);  //port
connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

char recvBuf[BUFFER_SIZE];
recv(sock,recvBuf,BUFFER_SIZE,0);
printf("%s\n",recvBuf);

//---------recieve publickey and context-------------
char file_name[FILE_NAME_MAX_SIZE];  
bzero(file_name, sizeof(file_name));  
printf("Please Input File Name On Server.\t");  
scanf("%s", file_name);  

bzero(recvBuf, sizeof(recvBuf));  
strncpy(recvBuf, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));  
send(sock,recvBuf, BUFFER_SIZE, 0);  

FILE *fp = fopen(file_name, "w");  
bzero(recvBuf, sizeof(recvBuf));  
int length=0;
//continue writing until the end of the file 
while((length=recv(sock,recvBuf,BUFFER_SIZE,0))>0){
    cout<<"receving"<<endl;
    fwrite(recvBuf,sizeof(char),length,fp);
    bzero(recvBuf, BUFFER_SIZE);  
}     
printf("Recieve File:\t %s From Server Finished!\n", file_name);  
fclose(fp);

//---------recieve publickey finished-------------
close(sock);

    return 0;
 } 

