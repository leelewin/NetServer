#ifndef SERVICE_PROCESS_H_
#define SERVICE_PROCESS_H_

#define SERVER "Server: LMltiny/2.0/r/n"

void accept_request(int *arg);
int get_line(int client, char *buff, int size);
void ret_message(int client);
#endif
