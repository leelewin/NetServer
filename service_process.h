#ifndef SERVICE_PROCESS_H_
#define SERVICE_PROCESS_H_

#define SERVER "Server: LMltiny/2.0/r/n"

void accept_request(int *arg);
int get_line(int client, char *buff, int size);
void unimplement(int client);
int analysis_url(char *url, char *filename, char *argstr);
void not_found(int client);
void headers(int client, const char *filename);
void cat(int client, FILE *resource);
void serve_file(int client, char *filename);
#endif