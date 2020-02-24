#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>

int startup(u_short *);
int get_line(int client, char *buff, int size);





/****************************************/
/*Function:从已连接套接字接收请求报文并解析
 *
 *Parameters:指向套接字变量的指针
 *
 *Return:无
 *
****************************************/
void accept_request(int *arg){
    int client = *arg;
    int numchar = 0; //接收的字符数
    char buff[2048]; //存储读出的一行数据

    numchar = get_line(client, buff, sizeof(buff));

    printf("%d\n", numchar);
    printf("%s", buff);









}

/****************************************/
/*Function:从指定套接字获取一行
 *
 *Parameters:指定的套接字
             存储数据的地址
             数据区的大小
 *
 *Return:获取的字符个数
 *
*****************************************/
int get_line(int client, char *buff, int size){
    char c = '\0';
    int i = 0;
    int n = 0;

    while((i < size - 1) && (c != '\n')){

        n = recv(client, &c, sizeof(c), 0);
        if(n > 0){
            buff[i] = c;
            i++;
        }
        else
            c = '\n';

    }
    buff[i] = '\0';

    return i;
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
/*
        printf("start to read -----\n");


        //从客户端读数据
        if(read(client_socket, buff, sizeof(buff)) == -1){
            perror("send");
            exit(1);

        }
        printf("message = %s\n", buff);
*/
        
        accept_request(&client_socket);

        close(client_socket);

    }

    return 0;
}
