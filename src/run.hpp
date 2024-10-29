// duipai
#ifndef __RUN_HPP__
#define __RUN_HPP__
#include <bits/stdc++.h>
#include <sys/time.h>
#include <thread>
#include <mutex>
#include "base.hpp"
#include <unistd.h>
using namespace std;
bool acc[1000005];
int threadCnt,killed,testCaseId[1000005];
bool spj=0,limitt=0,limitc=0;
int Caselimit=500;
double Timelimit=45;
double timeUsed[1000005];
mutex killedMtx;
thread t[1000005];
unsigned long long t0,t1;
unsigned long long gettime(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long long noww,now;
    return tv.tv_sec*1000+tv.tv_usec/1000;
}
void printStat()
{
    system("clear");
    int tot=0;
    for(int i=1;i<=threadCnt;i++)
    {
        if(!acc[i]) printf("\033[1mthread #%d\033[0m\n\033[1;32mRunning\033[0m on case \033[34m%d\033[0m.\n\033[34m%.3lfs\033[0m have been used.\n\n",i,testCaseId[i],timeUsed[i]),tot+=testCaseId[i];
        else printf("\033[1mthread #%d\033[0m\n\033[32mAccept\033[0m.\n\n",i);
    }
    printf("\033[1mtotal time:\033[0m \033[34m%.3lfs\033[0m\n",(gettime()-t0)*1.0/1000);
    printf("\033[1mtotal case:\033[0m \033[34m%d\033[0m\n\n",tot);
}
inline void threadFunction(int &cnt,int id,bool &accepted,double &tm)
{
    unsigned long long noww,now;
    noww=now=gettime();
    while(((!(limitc^limitt) && cnt<=Caselimit && (noww-now)*1.0/1000<=Timelimit) || (limitc && cnt<=Caselimit) || (limitt && (noww-now)*1.0/1000<=Timelimit)) && !killed)
    {
        ++cnt;
        noww=gettime();
        tm=(noww-now)*1.0/1000;
        system(("./make_data > data"+to_string(id)+".txt").c_str());
        system(("./ans < data"+to_string(id)+".txt > ans"+to_string(id)+".txt").c_str());
        system(("./my < data"+to_string(id)+".txt > my"+to_string(id)+".txt").c_str());
        string diff;
        if(!spj) diff="diff -w ans"+to_string(id)+".txt my"+to_string(id)+".txt";
        else diff="./spj <my"+to_string(id)+".txt";
        if(system(diff.c_str()))
        {
            killedMtx.lock();
            accepted=1;
            if(killed){
                killedMtx.unlock();
                return ;
            }
            killed=id;
            killedMtx.unlock();
            return ;
        }
        if((gettime()-t1)*1.0/1000>0.1) t1=gettime(),printStat();
    }
    accepted=1;
    return ;
}
inline void comp(const string &file,const vector<string> &arg);
inline void test(vector<string> &arg)
{
    threadCnt=1,spj=0,limitt=limitc=0,Caselimit=500,Timelimit=45;
    for(int i=1;i<arg.size();i++)
    {
        if(arg[i]=="-c" || arg[i]=="--cores") threadCnt=stoi(arg[i+1]);
        else if(arg[i]=="-s" || arg[i]=="--spj") spj=1;
        else if(arg[i]=="-T" || arg[i]=="--Timelimit") limitt=1,Timelimit=stod(arg[i+1]);
        else if(arg[i]=="-C" || arg[i]=="--Caselimit") limitc=1,Caselimit=stoi(arg[i+1]);
    }
    killed=0;
    killedMtx.unlock();
    t0=t1=gettime();
    for(int i=1;i<=threadCnt;i++)
    {
        testCaseId[i]=1;
        acc[i]=0;
        timeUsed[i]=0;
        t[i]=thread(threadFunction,ref(testCaseId[i]),i,ref(acc[i]),ref(timeUsed[i]));
    }
    while(!killed)
    {
        bool flag=true;
        for(int i=1;i<=threadCnt;i++) flag&=acc[i];
        if(flag) break;
    }
    for(int i=1;i<=threadCnt;i++) t[i].join();
    system("clear");
    if(killed)
    {
        printf("thread #%d : \033[1;31mWrong answer\033[0m on test \033[34m%d\033[0m.\n\n",killed,testCaseId[killed]);
        system(("cp ans"+to_string(killed)+".txt ans.txt").c_str());
        system(("cp data"+to_string(killed)+".txt data.txt").c_str());
        system(("cp my"+to_string(killed)+".txt my.txt").c_str());
        compout();
    }
    else printf("\033[32mAccept\033[0m.\n\n");
    for(int i=1;i<=threadCnt;i++)
    {
        system(("rm ans"+to_string(i)+".txt").c_str());
        system(("rm data"+to_string(i)+".txt").c_str());
        system(("rm my"+to_string(i)+".txt").c_str());
    }
}
#endif