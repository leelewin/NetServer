#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<ctype.h>
#include<strings.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<time.h>
#include"service_process.h"


#define BUFFLEN 4096
#define MAX_EVENTS 512

int startup(u_short *);
struct event_s 
{
    int fd;           //要监听的文件描述符
    int events;
    void *arg;        //指向自己结构体指针
    void (*call_back)(int fd, int events, void *arg);  //回调函数
    int status;       //是否在红黑树上
    char buff[BUFFLEN];
    int len;
    long last_active;   //加入红黑树的时间
};

int g_root;                                            //红黑树的根结点
struct event_s g_events[MAX_EVENTS+1];                //???


//初始化结构体
void event_set(struct event_s *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg){
    ev->fd = fd;
    ev->events = 0;
    ev->arg = arg;
    ev->call_back = call_back;
    ev->status = 0;
    memset(ev->buff, 0, sizeof(ev->buff));
    ev->len = 0;
    ev->last_active = time(NULL);                    //记录上树的时间

    return ;

}

void event_add(int root, int event, struct event_s *ev){
    struct epoll_event epv = {0, {0}};
    int op = 0;                               //有问题

    epv.data.ptr = ev;
    epv.events = event;
    ev->events = event;

    if(ev->status == 0){
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    epoll_ctl(root, op, ev->fd, &epv);

    return ;

}

//从红黑树中删除一个文件描述符
void event_del(int root, struct event_s *ev){
    struct epoll_event epv = {0, {0}};

    if(ev->status == 0)
        return ;
    epv.data.ptr = NULL;
    ev->status = 0;                                //修改状态为未在树上
    epoll_ctl(root, EPOLL_CTL_DEL, ev->fd, &epv);

    return;

}

void receive_data(int fd, int events, void *arg){


}
void send_data(){


}

void process_connect(int fd, int events, void *arg){
    int client_socket = -1;
    struct sockaddr_in client_name;
    socklen_t client_len = sizeof(client_name);
    int i;

    client_socket = accept(fd, (struct sockaddr *)&client_name, &client_len);         //将监听套接字转化为已连接套接字
    do{
        for(i=0; i < MAX_EVENTS; i++)         //从数组g_events挑出一个空闲的元素
            if(g_events[i].status == 0)
                break;
        if(i == MAX_EVENTS)
            break;
        fcntl(client_socket, F_SETFL, O_NONBLOCK);

        event_set(&g_events[i], client_socket, receive_data, &g_events[i]);
        event_add(g_root, EPOLLIN, &g_events[i]);

        return;


    }while(0);

    return;

}

/*********************************************/
/*Function:在指定端口监听连接
 *
 *Parameters:指向要连接端口变量的指针
 *
 *Return: 监听套接字描述符
*/
/********************************************/
int startup(u_short *port){
    int sock_d = 0;
    struct sockaddr_in name;
    int on = 1;

    sock_d = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_d < 0){
        perror("socket");
        exit(1);
    }
    fcntl(sock_d, F_SETFL, O_NONBLOCK);        //设置为非阻塞套接字
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if((setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0){
        perror("setsockopt");
        exit(1);
    }
    if(bind(sock_d, (struct sockaddr *)&name, sizeof(name)) < 0){
        perror("bind");
        exit(1);
    }
    if(listen(sock_d, 512) < 0){
        perror("listrn");
        exit(1);
    }
    
    //返回监听套接字描述符
    return sock_d;
}

int main(){
    u_short port = 8000;
    int server_socket = -1;
    //int client_socket = -1;
    //struct sockaddr_in client_name;
    //socklen_t client_name_len = sizeof(client_name);

    //char buff[100];

    g_root = epoll_create(MAX_EVENTS+1);



    server_socket = startup(&port);
    printf("The server is running now in %d port\n", port);
//void event_set(struct event_s *ev, int fd, void (*call_back)(int fd, int events, void *arg), void *arg){
    event_set(&g_events[MAX_EVENTS], server_socket, process_connect, &g_events[MAX_EVENTS]);
//void event_add(int root, int event, struct event_s *ev){
    event_add(g_root, EPOLLIN, &g_events[MAX_EVENTS]);

    struct epoll_event ret_events[MAX_EVENTS];         //保存已经满足条件的文件描述符数组
    int check_pos = 0;

    while(1){
        //超时验证
        //
        //
        //ggg
        //
        //
        int wait_ret = epoll_wait(g_root, ret_events, MAX_EVENTS+1, 1000);
        if(wait_ret < 0){
            perror("epoll_wait");
            break;
        }
        for(int i=0; i<wait_ret; i++){
            struct event_s *ev = (struct event_s *)ret_events[i].data.ptr;
            if((ret_events[i].events & EPOLLIN) && (ev->events & EPOLLIN)){  //读事件满足
                ev->call_back(ev->fd, ret_events[i].events, ev->arg);           //回调含糊
            }
            if((ret_events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)){ //写事件满足
                ev->call_back(ev->fd, ret_events[i].events, ev->arg);
            }
        }
    }
    return 0;
}
