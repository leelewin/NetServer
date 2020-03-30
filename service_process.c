
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<ctype.h>
#include<strings.h>

#define SERVER "Server: LMltiny/2.0/r/n"
int return_tag = 0;  //返回客户端成功与否的标志

void accept_request(int *arg);
int get_line(int client, char *buff, int size);
void ret_message(int client);


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
    char argstr[1024]; //存储客户机传递过来的参数
    int i = 0;
    int j;
    return_tag = 0;

    numchar = get_line(client, buff, sizeof(buff));

    while(!isspace((int)buff[i]) && (i < numchar - 1)){
        method[i] = buff[i];
        i++;
    }
    method[i] = '\0';

    //不是GET方法则返回客户端无法实现
    if(strcasecmp(method, "GET")){
       // memset(buff, 0, sizeof(buff));
        while((numchar > 0) && strcmp("\n", buff)){
            numchar = get_line(client, buff, sizeof(buff));
        }
        ret_message(client);
        close(client);
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

    //将GET方法传递的信息解析出来
    char *query_string = url;
    while((*query_string != '?') && (*query_string != '\0'))
        query_string++;

    if(*query_string == '?'){
        *query_string = '\0';
        strcpy(argstr, query_string + 1);
    }
    printf("argstr %s\n", argstr);  //模拟解析argstr包含的数据并插入到数据库中
    //解析成功return_tag置为一
    return_tag = 1;

    while((numchar > 0) && strcmp("\n", buff)){
        numchar = get_line(client, buff, sizeof(buff));
    }
    ret_message(client);
    close(client);

}

//************************************/
//
//
//************************************/

void ret_message(int client){
    char buf[1024];

    if(return_tag == 0){     //失败返回客户端
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
    else{                   //成功返回客户端
        strcpy(buf, "HTTP/1.1 200 OK\r\n");
        send(client, buf, strlen(buf), 0);
        sprintf(buf, "Content-Type: text/html\r\n");
        send(client, buf, strlen(buf), 0);
        strcpy(buf, "\r\n");
        send(client, buf, strlen(buf), 0);

    }

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
            if(c == '\r'){//修复bug：\r\n无法结束
                n = recv(client, &c, sizeof(c), MSG_PEEK);
                if(n > 0 && c == '\n')
                    recv(client, &c, sizeof(c), 0);
                else
                    c = '\n';
            }                                          
            buff[i] = c;
            i++;
        }
        else
            c = '\n';

    }
    buff[i] = '\0';

    return i;
}
