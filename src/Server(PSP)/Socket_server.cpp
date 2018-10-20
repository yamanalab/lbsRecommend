//encryption header file setting

#include "FHE.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <time.h>

//socket header file setting
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

#define FILE_NAME_MAX_SIZE 20
#define BUFFER_SIZE 4096

using namespace std;
int main(int argc, char **argv)
{
//FHE setting ----------------------------------
long m=16384, p=8191, r=3;
long L=10;
long c=3;
long w=64;
ZZX G;
cout<< "Found M = " << m << endl;
FHEcontext context(m, p, r);
// initialize context
buildModChain(context, L, c);
FHESecKey secretKey(context);
const FHEPubKey& publicKey = secretKey;
G = context.alMod.getFactorsOverZZ()[0];
secretKey.GenSecKey(w);
addSome1DMatrices(secretKey);
cout << "Generated key" << endl;
EncryptedArray ea(context, G);
//FHE setting finished-------------------------

/* ---------- Public Key Writing ------ */
ofstream pkFile("/home/lyuqiuyi/pk");
writeContextBase(pkFile,context);
pkFile << context << endl;
pkFile << publicKey << endl;
pkFile.close();
/* ---------- Secret Key Writing ------ */
ofstream skFile("/home/lyuqiuyi/sk");
writeContextBase(skFile,context);
skFile << context << endl;
skFile << secretKey << endl;
skFile.close();


//socket setting ----------------------------------
//create socket
int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//bind 
struct sockaddr_in serv_addr;
//set all the bits to '0'
memset(&serv_addr, 0, sizeof(serv_addr)); 
//use IPv4 addreass
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr("192.168.101.3");  //ip addresss of server enigma01
serv_addr.sin_port = htons(1234);  //port
bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//listen and wait for the client
listen(serv_sock, 20);
 
while(1){   
//recieve 
struct sockaddr_in clnt_addr;
socklen_t clnt_addr_size = sizeof(clnt_addr);
int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
char sendBuf[BUFFER_SIZE];
socklen_t length = sizeof(clnt_addr);
sprintf(sendBuf, "connect is successful");
send(clnt_sock,sendBuf,strlen(sendBuf)+1,0);

//---------send context and publickey start-------------
//start sending
char buffer[BUFFER_SIZE];
bzero(buffer, BUFFER_SIZE);  
length = recv(clnt_sock, buffer, BUFFER_SIZE, 0);  
if (length < 0)  
 {  
     printf("Server Recieve Data Failed!\n");  
     break;  
  }  

char file_name[FILE_NAME_MAX_SIZE];
bzero(file_name,FILE_NAME_MAX_SIZE);
strncpy(file_name, buffer,strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

FILE *fp = fopen(file_name, "r");
if (fp == NULL)  {
  printf("File:\t%s Not Found!\n", file_name);
}else {
   bzero(buffer,BUFFER_SIZE);
   int length = 0;
   while((length = (int)fread(buffer, sizeof(char), BUFFER_SIZE, fp))>0)
   {
     send(clnt_sock,buffer,length,0);
     bzero(buffer,sizeof(buffer));
     cout<<"transfering"<<endl; 
   // file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
   }
   fclose(fp);  
   printf("File:\t%s Transfer Finished!\n", file_name);  
//---------send context and  publickey finished-------------
   }
   close(clnt_sock);
}   
 close(serv_sock); 
 return 0;

}

