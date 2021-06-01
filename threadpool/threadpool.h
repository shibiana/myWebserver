#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"

//线程池类包含的内容
template<typename T>
class threadpool 
{
public:
    //max_request 表示请求队列中最多允许的，等待处理的请求的数量
    threadpool(int actor_model,connection_pool* connPool, int thread_number = 8, int max_request = 10000);
    ~threadpool();

    bool append(T* request, int state);
    bool append(T* request);
private:
    //工作线程运行的函数，不断从工作队列中取出人物并执行
    static void* worker(void* arg);
    void run();

private:
    int m_thread_number;            //线程池中的线程数
    int m_max_requests;             //请求队列中允许的最大请求数
    pthread_t* m_threads;           //描述线程池的数组，其大小为m_thread_number
    std::list<T*> m_workqueue;      //请求队列
    locker m_queuelocker;           //保护请求队列的互斥锁
    sem m_queuestat;                //是否有任务需要处理
    connection_pool* m_connPool;    //数据库
    int m_actor_model;              //模型切换

};

template<typename T>
threadpool<T>::threadpool(int actor_model,connection_pool* connPool, int thread_number, int max_request):m_actor_model(actor_model),m_thread_number(thread_number),m_max_requests(max_request),m_threads(NULL),m_connPool(connPool)
{
    //如果线程数 和 最大请求数 <= 0
    if(thread_number <= 0 || max_request <= 0)
    {
        throw std::exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads)
        throw std::exception();
    //创建thread_number数量的线程池
    for(int i = 0; i < thread_number; i++)
    {
        if(pthread_create(m_threads + i,NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }

        //线程分离失败
        if(pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}

template<typename T>
bool threadpool<T>::append(T* request, int state)
{
    m_queuelocker.lock();
    if(m_workqueue.size() >= m_max_requests)
    {
        m.queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    //添加任务
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    //信号量提醒有任务要处理
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::worker(void* arg)
{
    threadpool* pool = (threadpool*) arg;
    pool->run();
    return pool;
}

//工作线程从请求队列中取出某个任务进行处理，需要注意线程同步
template<typename T>
void threadpool<T>::run()
{
    while(true)
    {
        //等待任务
        m_queuestat.wait();
        m.queuelocker.lock();
        if(m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if(!request)
            continue;
        //从连接池中取出一个与数据库进行连接

        //process（模板类中的方法，http类）进行处理

        //将数据库连接池放回连接池
    }
}

#endif