#ifndef LOCKER_H
#define LOCKER_H
//线程同步类封装
#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
public:
    sem()  //信号量初始化
    {
        //第二个参数：信号量在线程间共享  第三个参数 信号量的初始值
        if(sem_init(&m_sem,0,0) != 0)   
        {
            throw std::exception();
        }
    }

    sem(int num)  //信号量初始化
    {
        //第二个参数：信号量在线程间共享  第三个参数 信号量的初始值
        if(sem_init(&m_sem,0,num) != 0)   
        {
            throw std::exception();
        }
    }

    ~sem()
    {
        sem_destroy(&m_sem);
    }

    bool wait()
    {
        return sem_wait(&m_sem) == 0;
    }

    bool post()
    {
        return sem_post(&m_sem) == 0;
    }


private:
    sem_t m_sem;   //信号量成员

};

class locker
{
public:
    locker()
    {
        if(pthread_mutex_init(&m_mutex,NULL) != 0)
        {
            throw std::exception();
        }
    }

    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    pthread_mutex_t* get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

//条件变量
class cond
{
public:
    cond()
    {
        if(pthread_cond_init(&m_cond,NULL) != 0)
        {
            throw std::exception();
        }
    }

    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }

    bool wait(pthread_mutex_t* m_mutex)
    {
        return pthread_cond_wait(&m_cond,m_mutex) == 0;
    }

    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        return pthread_cond_timedwait(&m_cond,m_mutex,&t) == 0;
    }

    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }

    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
private:
    pthread_cond_t m_cond;
};


#endif