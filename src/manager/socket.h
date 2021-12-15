#ifndef _MANAGEMENT_H
#define _MANAGEMENT_H


// int cmd_remove(char** out_buf, const char* client_id);
// int cmd_status(char** out_buf);

int socket_connect(int port, int* out_socket);
int send_command(char** out_buf, int buf_len, int socket, const char* COMMAND);
int send_command_no_output(int socket, const char* COMMAND);
#endif