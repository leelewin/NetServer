#include <stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>


int main()
{
    int sock_c = -1;
    struct sockaddr_in client_name;
    u_short port = 8000;
    int result = -1;
    char ch[60] = "is the begain";

    if((sock_c = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
    memset(&client_name, 0, sizeof(client_name));
    client_name.sin_family = AF_INET;
    client_name.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_name.sin_port = htons(port);

    result = connect(sock_c, (struct sockaddr *)&client_name, sizeof(client_name));
    if(result == -1){
        perror("connect");
        exit(1);
    }
    if(send(sock_c, ch, sizeof(ch), 0) == -1){
        perror("send");
        exit(1);
    }
    close(sock_c);




    
    return 0;
}

