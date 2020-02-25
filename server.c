#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<ctype.h>
#include<strings.h>

#define SERVER "Server: LMltiny/2.0"

int startup(u_short *);
void accept_request(int *arg);
int get_line(int client, char *buff, int size);
void unimplement(int client);
int analysis_url(char *url, char *filename, char *argstr);
void not_found(int client);
void headers(int client, const char *filename);
void cat(int client, FILE *resource);
void serve_file(int client, char *filename);


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
    int numchar = 0; //读取一行接收的字符数
    char buff[1024]; //存储读出的一行数据
    char method[512]; 
    char url[1024];
    char filename[1024];
    char argstr[1024];
    int i = 0;
    int j;
    int is_static = 0;  //is_static为0访问的静态内容，为1访问的动态内容

    numchar = get_line(client, buff, sizeof(buff));

    printf("%d\n", numchar);
    printf("%s\n", buff);

    while(!isspace((int)buff[i]) && (i < numchar - 1)){
        method[i] = buff[i];
        i++;
    }
    method[i] = '\0';

    printf("%s\n", method);
    //不是GET 和 POST方法则返回客户端无法实现
    if(strcasecmp(method, "GET") && strcasecmp(method, "POST")){
        unimplement(client);
        return;
    }

    while(isspace((int)buff[i]) && i < numchar - 1)
        i++;

    j = 0;
    while(!isspace((int)buff[i]) && i < numchar - 1){
        url[j] = buff[i];
        i++;
        j++;
    }
    url[j] = '\0';

    is_static = analysis_url(url, filename, argstr);

    //printf("%d\n", is_static);
    //printf("%s\n", filename);
    //printf("%s\n", argstr);
    
    //待添加文件权限验证
    
    if(is_static == 0){
        serve_file(client, filename);

    }
    else{
        //execte_cgi(client, filename, argstr);

    }



}

/********************************************/



/********************************************/
void serve_file(int client, char *filename){
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];

    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
        numchars = get_line(client, buf, sizeof(buf));

    resource = fopen(filename, "r");
    if (resource == NULL)
        not_found(client);
    else
    {
        headers(client, filename);
        cat(client, resource);
    }
    fclose(resource);


}
/*************************************/


/*************************************/
void cat(int client, FILE *resource)
{
    char buf[1024];

    fgets(buf, sizeof(buf), resource);
    while (!feof(resource))
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}
//************************************/
//
//
//************************************/
void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

/****************************************/


/****************************************/
void not_found(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}
/**********************************************/
/*Function: 将url分割成filename和argstr两部分
 *          对于url为'/'情况添加默认的index.html
 *
 *Parameters: 指向url的指针
              指向filename的指针
              指向argstr的指针
 *
 *return: 0 or 1
**********************************************/
int analysis_url(char *url, char *filename, char *argstr){
    char *query_string = url;

    while((*query_string != '?') && (*query_string != '\0'))
        query_string++;

    if(*query_string == '?'){//动态
        *query_string = '\0';
        strcpy(filename, url);
        strcpy(argstr, query_string + 1);
        return 1;
    }
    else{ //静态
        sprintf(filename, "htl%s", url);
        if(filename[strlen(filename)-1] == '/'){
            strcat(filename, "index.html");

        }
        strcpy(argstr, "");
        return 0;

    }
}
/***************************************************/
/*Function:通知客户端方法没有实现
*
*Parameters:已连接套接字
*
*Return:无
***************************************************/
void unimplement(int client){









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
        
        accept_request(&client_socket);

        printf("end\n");
        close(client_socket);

    }

    return 0;
}
