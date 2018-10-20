/*
 * This program has 1 task
 * 1.To decrypt the full recommendation list recieved from PPRS
 * */

#include "FHE.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
//setting FHE
    unsigned long m,p,r;
    vector<long> gens,ords;
    ifstream sk("/home/lyuqiuyi/fhe/work/sk");
    readContextBase(sk,m,p,r,gens,ords);
    FHEcontext context(m,p,r,gens,ords);
    sk >> context;
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;
    sk >> secretKey;
    sk.close();
    EncryptedArray ea(context);
    
    int sz=10;
    long nslots = ea.size();
    cout<<"the size of nslots is "<<nslots<<endl;

    if(nslots < sz){
        cerr << "nslots is too small!" << endl;
        exit(-1);
    }
//setting FHE finished

    vector<Ctxt> ctxts(sz, Ctxt(publicKey));
    ifstream finalresult("/home/lyuqiuyi/fhe/work/ctxt/ctxtFile10.txt");
    for(int i = 0; i < sz; i++) {
         finalresult >> ctxts[i];
    }

    cout<<"---------------- "<<endl;
    cout<<"The decrept result is "<<endl;
    for (int i=0; i<sz; i++){
        vector<long> res;
        ea.decrypt(ctxts[i], secretKey, res);
        cout << res[0]<<endl;
         }

 return 0;
}
               
