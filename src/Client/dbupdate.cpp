/*
 * This program has 1 task
 * 1. generate updating coocurrence matrix, encrypt and write it to a file
 * */
#include "FHE.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <time.h>

#define MAX 10
//Assumption: Item9 is selected by target user from partial recommendation list
#define newIndex 9
//Assumption: Item9's value is 3
#define newIndexValue 3

using namespace std;
int main(int argc, char **argv){
//setting FHE
unsigned long m,p,r;
    vector<long> gens,ords;
    ifstream ct("/home/lyuqiuyi/fhe/work/publicKey");
    readContextBase(ct,m,p,r,gens,ords);
    FHEcontext context(m,p,r,gens,ords);
    ct >> context;
    FHEPubKey publicKey(context);
    ct >> publicKey;
    ct.close();
    EncryptedArray ea(context);

    int sz=10;

    long nslots = ea.size();
    cout<<"the size of nslots is "<<nslots<<endl;

    if(nslots < sz){
        cerr << "nslots is too small!" << endl;
        exit(-1);
    }

//setting FHE finished

//co-occurence for updating
int co[MAX][MAX]={0};

vector<long> userprefer = {1,3,5,1,2,0,0,0,0,0};
cout<<"---------------- "<<endl;
cout<<"The vector of user's original preference is "<<endl;
for(int j = 0; j < MAX; j++) {
      cout<<userprefer[j]<<" ";
}
cout<<endl;

for(int i=0;i<MAX;i++){
  if(userprefer[i]!=0){
     co[i][newIndex]=1;
     co[newIndex][i]=1;
    }
}
co[newIndex][newIndex]=1;

cout<<"---------------- "<<endl;
cout<<"The co-occurence for updating is "<<endl;
for(int i = 0 ; i < MAX; i++) {
  for(int j = 0; j < MAX; j++) {
       cout<<co[i][j]<<"   ";
   }
        cout<<endl;
}

//encrypting
cout<<"---------------- "<<endl;
cout<<" Start to encrypt...... "<<endl;

vector<vector<long>> mat(sz, vector<long>(nslots));
for(int i = 0 ; i < sz; i++) {
for(int j = 0; j < sz; j++) {
   mat[i][j]=n[i][j];
 }
}

vector<Ctxt> ctxts(sz, Ctxt(publicKey));
for(int i = 0; i < sz; i++) {
    ea.encrypt(ctxts[i], publicKey, mat[i]);
}

//write to the file
ofstream ctxtFile("/home/lyuqiuyi/fhe/work/dbupdate/dbupdate10.txt");
for(auto&& ctxt: ctxts) {
 ctxtFile << ctxt << endl;
}

//new user preference list
userprefer[newIndex]=newIndexValue;
cout<<"---------------- "<<endl;
cout<<"The vector of user's new preference is "<<endl;
for(int j = 0; j < MAX; j++) {
      cout<<userprefer[j]<<" ";
}
cout<<endl;

}           
