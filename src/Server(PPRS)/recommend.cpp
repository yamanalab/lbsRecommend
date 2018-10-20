/*
 * This program has 1 task:
 * 1.do multiplication between encrypted co-occurence matrix and encrypted userprefer vector, write the encrypted result to the file
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

//read encrypted co-occurence matrix from the file
    vector<Ctxt> ctxts(sz, Ctxt(publicKey));
    ifstream coocurrence("/home/lyuqiuyi/fhe/work/coFile/coFileEnc10.txt");
    for(int i = 0; i < sz; i++) {
         coocurrence >> ctxts[i];
    }

//read encrypted userpreference  
    Ctxt vecCtxt(publicKey);
    ifstream ctxtFile("/home/lyuqiuyi/fhe/work/userprefer/userprefer10.txt");
    ctxtFile >> vecCtxt;

cout<<"Multiply..."<<endl;
    
//for counting the time
clock_t start2,finish2;
double totaltime2;
start2=clock(); 

// multiply
    vector<Ctxt> results(sz, Ctxt(publicKey));
    for (int i = 0; i < sz; i++) {
        results[i] = ctxts[i];  //  copy
        results[i].multiplyBy(vecCtxt);
        // sum row
        totalSums(ea, results[i]);
    }
    
//for counting the time
finish2=clock();
totaltime2=(double)(finish2-start2)/CLOCKS_PER_SEC;
cout<<"\nthe time for multiplication is "<<totaltime2<<"seconds!"<<endl; 
    
//writing the encrypted result to the file
    ofstream ctxtFile2("/home/lyuqiuyi/fhe/work/ctxt/ctxtFile10.txt");
    for(auto&& ctxt: results)
    {
      ctxtFile2 << ctxt << endl;
    }   
    return 0;
}
