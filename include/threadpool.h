#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#define DEFAULT_TIME 10              //管理者线程管理线程池的工作周期
#define DEFINE_THREAD_VARY 10        //每次创建和销毁的线程个数
#define false 0
#define true 1

//任务队列的类型
typedef struct task{
    void *arg;
    void *(*function)(void *);      //回调函数
}threadpool_task_t;

//描述线程池的信息
typedef struct threadpool_t{
    pthread_mutex_t locker;         //锁住本结构体
    pthread_mutex_t thread_count;   //锁住忙线程

    pthread_cond_t queue_full;      //队列满时，向队列添加任务的线程阻塞
    pthread_cond_t queue_empty;     //队列空时，向队列取任务的线程阻塞

    pthread_t *work_thread;         //存放线程池中每个线程的tid
    pthread_t manage_thread;        //存放线程池管理者的tid
    int min_thr_num;                //最小线程数
    int max_thr_num;                //最大线程数
    int live_thr_num;               //存活的全部线程数
    int busy_thr_num;               //忙的线程数
    int wait_exit_num;              //等待销毁的线程数

    threadpool_task_t *task_queue;  //指向任务队列的首地址
    int queue_front;
    int queue_rear;
    int queue_size;                 //任务队列的实际任务数
    int queue_max_size;             //任务队列的最大数

    int shutdown;                   //标志位，标识线程的状态
}threadpool_t;

/*功能：创建线程池
  参数：min_thr_num 线程池的最少线程数
        max_thr_num 线程池的最大线程数
        queue_max_size  任务队列的最大值
  返回值：指向线程池结构体的指针 
*/
extern threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);
//向任务队列添加任务
extern int threadpool_add(threadpool_t *pool, void *(*function)(void *), void *arg);
//销毁线程池
extern int threadpool_destroy(threadpool_t *pool);
#endif
