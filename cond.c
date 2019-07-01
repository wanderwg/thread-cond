#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

//实现条件变量的基本使用，以自己吃面和厨师做面为例：
//顾客想要吃面，前提是有面，没有面则等待
//厨师做面，做好面后唤醒吃面的人
int have_noodle=0;
pthread_mutex_t mutex;
pthread_cond_t cond_eat;
pthread_cond_t cond_cook;

void* eat_noodle(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(have_noodle==0)
        {
            //没有面，就不能吃面
            //int pthread_cond_wait(pthread_cond_t* restrict cond,pthread_mutex_t* restrict mutex);
            //在顾客查看没有面的时候要做以下三个步骤：
            //pthread_cond_wait实现这三个操作
            //1、解锁，不然厨师没法进入去做面
            //2、进入等待
            //3、被唤醒后加锁
            //其中解锁与休眠操作必须是原子操作
            pthread_cond_wait(&cond_eat,&mutex);
        }
        //到这一步的话说明有面
        printf("eat noodle~~\n");
        have_noodle=0;
        pthread_cond_signal(&cond_cook);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* cook_noodle(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(have_noodle==1)
        {
            //现在有面，没人吃，不用继续做
            pthread_cond_wait(&cond_cook,&mutex);
        }
        //此时没有面，需要做面
        printf("cook the noodle\n");
        have_noodle=1;
        pthread_cond_signal(&cond_eat);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
int main()
{
    pthread_t tid1,tid2;
    pthread_mutex_init(&mutex,NULL);
    //int pthread_cond_init(pthread_cond_t* cond,const pthread_condattr_t* attr);
    pthread_cond_init(&cond_eat,NULL);
    pthread_cond_init(&cond_cook,NULL);
    int i;
    for(i=0;i<4;++i)
    {
        int ret=pthread_create(&tid1,NULL,eat_noodle,NULL);
        if(ret!=0)
        {
            printf("pthread create error!\n");
            return -1;
        }
    }
    for(i=0;i<4;++i)
    {
        int ret=pthread_create(&tid2,NULL,cook_noodle,NULL);
        if(ret!=0)
        {
            printf("pthread create error!\n");
            return -1;
        }
    }
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_eat);
    pthread_cond_destroy(&cond_cook);
    return 0;
}
