#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include"threadpool.h"

void *threadpool_thread(void *threadpool);
void *threadpool_manage(void *threadpool);

int is_thread_alive(pthread_t tid){
    if(tid != 0)  //修改
    {    
        if(pthread_kill(tid, 0) == ESRCH){
            pthread_join(tid, NULL);                      //回收瘦身结束的线程，防止内存泄漏
            return false;

        }                                                   //测试线程是否还活着
    }   
    return true;
}
int threadpool_free(threadpool_t *pool){
    if(pool == NULL)
        return -1;
    if(pool->task_queue)
        free(pool->task_queue);
    if(pool->work_thread){
        free(pool->work_thread);
        pthread_mutex_lock(&(pool->locker));
        pthread_mutex_destroy(&(pool->locker));
        pthread_mutex_lock(&(pool->thread_count));
        pthread_mutex_destroy(&(pool->thread_count));
        pthread_cond_destroy(&(pool->queue_full));
        pthread_cond_destroy(&(pool->queue_empty));
    }
    free(pool);
    pool = NULL;

    return 0;
}

threadpool_t *threadpoll_create(int min_thr_num, int max_thr_num, int queue_max_size){

    threadpool_t *pool = NULL;
    do{
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL)
            break;
        
        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->live_thr_num = min_thr_num;
        pool->busy_thr_num = 0;
        pool->wait_exit_num = 0;

        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->queue_size = 0;                                           //队列的实际任务数
        pool->queue_max_size = queue_max_size;
        pool->shutdown = false;                                         //标志位，是否销毁线程池

        //给工作线程开辟空间
        if((pool->work_thread = (pthread_t *)malloc(max_thr_num * sizeof(pthread_t))) == NULL)
            break;
        memset(pool->work_thread, 0, sizeof(pthread_t) * max_thr_num);

        //给任务队列开辟空间
        if((pool->task_queue = (threadpool_task_t *)malloc(queue_max_size * sizeof(threadpool_task_t))) == NULL)
            break;

        //初始化互斥锁和条件变量
        if(pthread_mutex_init(&pool->locker, NULL) != 0
                    || pthread_mutex_init(&pool->thread_count, NULL) != 0
                    || pthread_cond_init(&pool->queue_full, NULL) != 0
                    || pthread_cond_init(&pool->queue_empty, NULL) != 0
                    )
            break;

        //启动min_thr_num个工作线程
        for(int i = 0; i < min_thr_num; i++)
            pthread_create(&(pool->work_thread[i]), NULL, threadpool_thread, (void *)pool);

        //启动管理者线程
        pthread_create(&(pool->manage_thread), NULL, threadpool_manage, (void *)pool);

        return pool;

    }while(0);

    //创建失败，释放pool分配的内存
    threadpool_free(pool);

    return NULL;


}

//线程池中的各个工作线程
void *threadpool_thread(void *threadpool){

    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;


    while(1){
        //访问结构体先加锁
        pthread_mutex_lock(&(pool->locker));

        while(pool->queue_size == 0 && !pool->shutdown){
            pthread_cond_wait(&(pool->queue_empty), &(pool->locker));
        //清理指定数目的空闲线程，如果要结束的线程数大于0，结束线程
            if(pool->wait_exit_num > 0){
                pool->wait_exit_num--;

                //如果线程池中的线程数大于最小值可以结束当前线程
                if(pool->live_thr_num > pool->min_thr_num){
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->locker));           
                    //pthread_detach(pthread_self());       //detach is_thread_alive会段错 解决在
                                                            //在is_thrad_alive 进行资源的回收
                    pthread_exit(NULL);                              //被骗退出
                }
            }
        }
        //如果指定了true,要关闭线程池里的每个线程，自行退出处理
        if(pool->shutdown){
            pool->live_thr_num--;
            pthread_mutex_unlock(&(pool->locker));

            pthread_detach(pthread_self());
            pthread_exit(NULL);                   //真正的销毁线程

        }
        //从工作队列中取出一个任务
        task.arg = pool->task_queue[pool->queue_front].arg;
        task.function = pool->task_queue[pool->queue_front].function;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--;        //任务队列实际任务数减一

        //通知可以有新任务加进来了
        pthread_cond_signal(&(pool->queue_full));

        pthread_mutex_unlock(&(pool->locker));

        //开始执行任务
        pthread_mutex_lock(&(pool->thread_count));
        pool->busy_thr_num++;                                //忙线程数加一
        pthread_mutex_unlock(&(pool->thread_count));

        (task.function(task.arg));                         //执行回调函数

        //任务处理结束
        pthread_mutex_lock(&(pool->thread_count));
        pool->busy_thr_num--;                             //忙线程数减一
        pthread_mutex_unlock(&(pool->thread_count));

    }
    pthread_exit(NULL);


}

void *threadpool_manage(void *threadpool){
    threadpool_t *pool = (threadpool_t *)threadpool;
    int i;

    while(!(pool->shutdown)){

        sleep(DEFAULT_TIME);                   //定期对线程池管理

        pthread_mutex_lock(&pool->locker);       //获取需要用到变量
        int queue_size = pool->queue_size;
        int live_thr_num = pool->live_thr_num;
        int min_thr_num = pool->min_thr_num;        //改动
        int max_thr_num = pool->max_thr_num;        //改动
        pthread_mutex_unlock(&(pool->locker));

        pthread_mutex_lock(&(pool->thread_count));
        int busy_thr_num = pool->busy_thr_num;
        pthread_mutex_unlock(&(pool->thread_count));
        //创建新线程  算法:任务数大于最小线程池个数，且存活的线程数小于最大线程个数
        //忙的线程数 * 5 > 活的线程数 * 4 80% 
        if(queue_size > min_thr_num && live_thr_num  < max_thr_num && busy_thr_num * 5 > live_thr_num * 4){    //改动
            pthread_mutex_lock(&(pool->locker));
            int add = 0;

            for(i = 0; i < pool->max_thr_num && add < DEFINE_THREAD_VARY
                             && pool->live_thr_num < pool->max_thr_num; i++){
                if(pool->work_thread[i] == 0 || !is_thread_alive(pool->work_thread[i])){  //查看线程是否活着来复用线程池位置

                    pthread_create(&(pool->work_thread[i]), NULL, threadpool_thread, (void *)pool);//这个是覆盖之前的线程id??
                    add++;
                    pool->live_thr_num++;

                }

            }
            pthread_mutex_unlock(&(pool->locker));
        }
        //销毁多余的空闲线程   算法 忙线程 *5 小于存活线程数-20%-  且存活的线程数大于最小线程数
        if((busy_thr_num * 5) < live_thr_num && live_thr_num > min_thr_num){       //改动
            //一次销毁DEFINE_THREAD_VARY个线程
            pthread_mutex_lock(&(pool->locker));
            pool->wait_exit_num = DEFINE_THREAD_VARY;
            pthread_mutex_unlock(&(pool->locker));

            for(i = 0; i < DEFINE_THREAD_VARY; i++){
                pthread_cond_signal(&(pool->queue_empty));
            }

        }

    }
    return NULL;


}



int threadpool_add(threadpool_t *pool, void *(*function)(void *), void *arg){
    pthread_mutex_lock(&(pool->locker));
     //如果工作队列满了阻塞在此处
    while(pool->queue_size == pool->queue_max_size && (!pool->shutdown))
        pthread_cond_wait(&(pool->queue_full), &(pool->locker));

    if(pool->shutdown){
        pthread_mutex_unlock(&(pool->locker));
        return -1;
    }
    //清空工作线程调用的回调函数的参数
    if(pool->task_queue[pool->queue_rear].arg != NULL)
        pool->task_queue[pool->queue_rear].arg = NULL;

    //添加任务到任务队列
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->task_queue[pool->queue_rear].function = function;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    pool->queue_size++;

    //添加完任务后，环形队列不为空，唤醒线程池中等待任务的线程
    pthread_cond_signal(&(pool->queue_empty));
    pthread_mutex_unlock(&(pool->locker));

    return 0;

}



int threadpool_destroy(threadpool_t *pool){
    int i;
    if(pool == NULL)
        return -1;
    pool->shutdown = true;
    //销毁管理者线程
    pthread_join(pool->manage_thread, NULL);

    for(i = 0; i < pool->live_thr_num; i++){
        pthread_cond_broadcast(&(pool->queue_empty));
    }

    while(pool->live_thr_num > 0);  //等待所有线程退出，再释放
    threadpool_free(pool);

    return 0;
}
