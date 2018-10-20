/*
 * This program has 1 task
 * 1.encrypt user-preference and write it to file
 * */
#include "FHE.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <time.h>

//the amount of POI is 10
#define MAX 10

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

    int sz=MAX;

    long nslots = ea.size();
    cout<<"the size of nslots is "<<nslots<<endl;

    if(nslots < sz){
        cerr << "nslots is too small!" << endl;
        exit(-1);
    }
//setting FHE finished
   
   vector<long> userprefer = {1,3,5,1,2,0,0,0,0,0};
   
   cout<<"---------------- "<<endl;
   cout<<"The vector of user's preference is "<<endl;
   for(int j = 0; j < sz; j++) {
      cout<<userprefer[j]<<" ";
    }
   cout<<endl;

//encrypt user-preference
   userprefer.resize(nslots, 0);
   Ctxt preferCtxt(publicKey);
   ea.encrypt(preferCtxt, publicKey, userprefer);

   ofstream ctxtFile("/home/lyuqiuyi/fhe/work/userprefer/userprefer10.txt");
   ctxtFile << preferCtxt << endl;

return 0;
}
