#ifndef WEBSERVER_H_
#define WEBSERVER_H_

//定义webserver类
#include <sys/socket.h>   //socket
#include <netinet/in.h>   //socketaddr_in  htons等
#include <arpa/inet.h>    //inet_pton等
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <string>
#include <iostream>

using namespace std;

//一些宏定义
const int MAX_FD = 65536;            //最大文件描述符
const int MAX_EVENT_NUMBER = 10000;  //最大事件数
const int TIMESLOT = 5;              //最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    //初始化函数
    void init(int port, string user, string passWord, string databaseName,int long_write,int opt_linger, int trigmode, int sql_num, int tread_num, int close_log, int actor_model);
    
    //线程池函数
    void thread_pool();

    //数据库连接池
    void sql_pool();

    //写日志
    void log_write();

    //trig_mode
    void trig_mode();

    void eventlisten();

    void eventloop();

    //定时器
    void timer(int connfd, struct sockaddr_in client_address);

    void adjust_timer(util_timer* timer);

    bool dealclientdata();

    bool dealwithsignal(bool& timeout, bool& stop_server);

    void dealwithwrite(int sockfd);

    void dealwithwrite(int sockfd);

public:
    int m_port;
    char* m_root;
    int m_log_write;
    int m_close_log;
    int m_actormodel;

    int m_pipefd[2];
    int m_epollfd;
    http_conn* users；

    //数据库相关
    connection_pool* m_connPool;

    //登录数据库的用户
    string m_user;
    //登录数据库的密码
    string m_passWord;
    //所使用的数据库的名字
    string m_databaseName;
    //连接的数量
    int m_sql_num;

    //线程池相关
    threadpool<http_conn>* m_pool;
    //连接的数量
    int m_thread_num;

    //event_loop相关
    epoll_event events[MAX_EVENT_NUMBER];

    int m_listenfd;
    int m_OPT_LINGER;
    int m_TRIGMode;
    int m_LISTENTrigmode;
    int m_CONNTrigmode;

    //定时器相关
    client_data *users_timer;
    Utils utils;

};

#endif