#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
#include"service_process.h"
#include"threadpool.h"

int startup(u_short *);
int nonblock(int fd);

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

int nonblock(int fd){
    int old_flag;
    int new_flag;

    old_flag = fcntl(fd, F_GETFD);
    new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFD, new_flag);
    return old_flag;

}

int main(){
    u_short port = 8000;
    int server_socket = -1;
    int client_socket = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

    char buff[10];

    int epfd;
    struct epoll_event event;


    printf("start up now\n");
    threadpool_t *pool = threadpool_create(5, 10 ,10);

    server_socket = startup(&port);
    printf("The server is running now in %d port\n", port);

    if((epfd = epoll_create(50)) < 0){
        perror("epoll_create");
        exit(1);
    }
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, server_socket, &event) < 0){
        perror("epoll_ctl");
        exit(1);
    }
    //监听套接字也可以非阻塞？？待测试


        int ret;
        struct epoll_event events[50];
        int i;
        int re;

    while(1){

        if((ret = epoll_wait(epfd, events, 50, -1)) == -1){
            perror("epoll_wait");
            exit(1);
        }
        for(i = 0; i < ret; i++){
            if(events[i].data.fd == server_socket){
                client_socket = accept(server_socket, 
                                       (struct sockaddr*)&client_name,
                                       &client_name_len);
                if(client_socket == -1){
                    perror("client_socket");
                    exit(1);
                }
                nonblock(client_socket); //socket设置为非阻塞
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_socket;
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, client_socket, &event) < 0){
                    perror("epoll_ctl_client");
                    exit(1);
                }
            }
            else if(events[i].data.fd == client_socket){
                //ET模式下不会被重复触发，把读缓冲的所有的数据读出
                threadpool_add(pool, accept_request, &client_socket);
                
                
                //accept_request(&client_socket);
                //close(client_socket);
                
                    
                

            }

        }

    }

    return 0;
}

