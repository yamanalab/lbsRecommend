/*
 * This program has 2 tasks
 * 1.generate co-ocurrence according to the users' preferences
 * 2.encrypt co-occurence and write it to file
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
using namespace std;
int main(int argc, char **argv){
//setting of FHE
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

//setting of FHE finished
int n[MAX][MAX]={0};
int N[MAX][MAX]={0};
//vector<vector<long>> N(sz, vector<long>(nslots));

ifstream infile("/home/lyuqiuyi/fhe/work/dataset/inputdata10.txt");
for(int i=0;i<MAX;i++){
   for(int j=0;j<MAX;j++){
    infile >> n[i][j];
 }
}

cout<<"---------------- "<<endl; 
cout<<"The userpreference in plaintext is "<<endl;
for(int i = 0 ; i < MAX; i++) {
  for(int j = 0; j < MAX; j++) {
       cout<<n[i][j]<<" ";
   } 
        cout<<endl;
 } 

//i is the row
//j is the column
for (int i=0;i<MAX;i++) {
for (int j=0;j<MAX;j++){
   if (n[i][j]!=0){
   N[j][j]+=1; 
   for(int k=j+1;k<MAX;k++){
       if(n[i][k]!=0){
        N[j][k]+=1;
        N[k][j]+=1; 
    }
   }
  }
 }
}

cout<<"---------------- "<<endl;
cout<<"The co-occurence matrix is "<<endl;
for(int i = 0 ; i < MAX; i++) {
  for(int j = 0; j < MAX; j++) {
       cout<<N[i][j]<<"   ";
   }
        cout<<endl;
 }

vector<vector<long>> mat(sz, vector<long>(nslots));
for(int i = 0 ; i < sz; i++) {
for(int j = 0; j < sz; j++) {
   mat[i][j]=N[i][j];
 }
}

//encrypt the co-occurence matrxi
vector<Ctxt> ctxts(sz, Ctxt(publicKey));
for(int i = 0; i < sz; i++) {
    ea.encrypt(ctxts[i], publicKey, mat[i]);
}
 
ofstream ctxtFile("/home/lyuqiuyi/fhe/work/coFile/coFileEnc10.txt");
for(auto&& ctxt: ctxts)
{
 ctxtFile << ctxt << endl;
}

return 0;

}
