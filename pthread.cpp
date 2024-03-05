#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <pthread.h>
#include <fstream>
#include <string>
#include <ctime>

void *thmain(void *arg);

std::string pickword(const std::string str,const std::string delimiter,int num);

std::string currenttime();
std::string filename_write;
int substrc(const std::string str,const std::string word);

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
std::ifstream infile;
std::ofstream outfile;
int main(int argc,char *argv[])
{
    std::cout<<"开始运行"<<std::endl;
    if(argc!=2)
    {
        std::cout<<"please using:./pthread1 text1.txt"<<std::endl;
        return 0;
    }
    std::string filename_read,current,line;
    char buffer[50]; 
    char timestamp[14];
    //std::ifstream infile;
    int line_count;
    filename_read = argv[1];
    current = currenttime();
    std::cout<<"current:"<<current<<"\ncurrent.length():"<<current.length()<<std::endl;
    current = current.substr(0,current.length()-2);
    snprintf(buffer,sizeof(buffer),"outfile_%s.txt",current.c_str());
    //snprintf(buffer,sizeof(buffer),"outfile_0001.txt",current.c_str());
    filename_write = (std::string)buffer;
    std::cout<<"filename_read:"<<filename_read<<std::endl;
    std::cout<<"filename_write:"<<filename_write<<std::endl;
    infile.open(filename_read.c_str());
    if(infile.is_open())
    {

        while(getline(infile,line))
        {
            line_count++;
        }
        std::cout<<"line"<<line_count<<":"<<line<<std::endl;
        infile.clear();
        infile.seekg(0);
        for (int ii = 0; ii < line_count; ii++)
        {
            pthread_t thid;
            if(pthread_create(&thid,NULL,thmain,(void *)filename_read.c_str())!=0)
            {
                std::cout<<"pthread_create failed"<<std::endl;
            }
        }
        sleep(1);
        pthread_mutex_destroy(&mutex);
    }
    else
    {
        std::cout<<"The file does not exist."<<std::endl;
        return 0;
    }

    
    std::cout<<"运行结束"<<std::endl;
    return 0;
}

void *thmain(void *arg)
{
    pthread_detach(pthread_self());                 //设置为分离模式，线程结束自动回溯资源
    std::string filename_read,line,word;
    char buffer[50];
    int Count,sum;
    filename_read = (std::string)(char*)arg;
    sum = 0;
    std::string delimiter = "|";
    size_t pos = 0;
    std::cout<<"thid:"<<pthread_self()<<std::endl;
    std::cout<<"filename_read:"<<filename_read<<std::endl;
    try
    {
        pthread_mutex_lock(&mutex);
        if(getline(infile,line));
        Count = substrc(line,delimiter)+1;
        std::cout<<"line:"<<line<<" Count:"<<Count<<std::endl;
        for(int i=0 ; i<Count; i++)
        {
            word = pickword(line,delimiter,i);
            std::cout<<"word:"<<word<<std::endl;
            sum+=atoi(word.c_str());
        }
        std::cout<<"sum:"<<sum<<std::endl;
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
        outfile.open(filename_write.c_str(),std::ios_base::app);//std::ios_base::app为追加写入模式
        if (outfile.is_open()) 
        {
            snprintf(buffer,sizeof(buffer),"sum:%-8dnum:%-10sthid:%d",sum,line.c_str(),pthread_self());
            outfile << buffer << std::endl;
            outfile.close();
        }
        pthread_mutex_unlock(&mutex);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    //sleep(2);
    //return 0;             //也可以
    pthread_exit(0);
}

std::string pickword(const std::string str, const std::string delimiter, int num)
{
    std::string Line,word;
    size_t pos=0;
    int NUM=0;
    Line = str;
    if((Line.find(delimiter,0)!=std::string::npos))
    {
        while(true)
        {
            pos = Line.find(delimiter,0);
            std::string subStr=Line.substr(0,pos);
            if(subStr==""||NUM==num)
            {
                word = subStr;
                break;
            }
            Line = Line.substr(pos+1);  
            NUM++;
        }
        return word;
    }
    else
    {
        return word;
    }
}

int substrc(const std::string str, const std::string word)
{
    std::string Str=str;
    int count=0;
    size_t pos;
    while((Str.find(word,0)!=std::string::npos))
    {
        pos = Str.find(word,0);
        Str = Str.substr(pos+1);
        count++;
    }
    return count;
}

std::string currenttime()
{
    char timestamp[15];
    std::time_t now = time(0);
    std::tm* now_tm = localtime(&now);
    snprintf(timestamp,sizeof(timestamp),"%04d%02d%02d%02d%02d%02d",now_tm->tm_year+1900,now_tm->tm_mon+1,now_tm->tm_mday,now_tm->tm_hour,now_tm->tm_min,now_tm->tm_sec);
    return (std::string)(char*)timestamp;
}