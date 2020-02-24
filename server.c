#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>

int startup(u_short *);

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

int main(){
    u_short port = 8000;
    int server_socket = -1;
    int client_socket = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

    char buff[100];


    printf("start up now\n");

    server_socket = startup(&port);
    printf("The server is running now in %d port\n", port);

    while(1){
        //返回已连接描述符
        client_socket = accept(server_socket, 
               (struct sockaddr *)&client_name, 
               &client_name_len);
        if(client_socket == -1){
            perror("accept");
            exit(1);
        }

        printf("start to read -----\n");

        if(read(client_socket, buff, sizeof(buff)) == -1){
            perror("send");
            exit(1);

        }
        printf("message = %s\n", buff);

        close(client_socket);

    }

    return 0;
}
