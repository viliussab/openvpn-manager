#ifndef _MANAGEMENT_H
#define _MANAGEMENT_H

int socket_connect(int port, int* out_socket);
int send_command(char** out_buf, int buf_len, int socket, const char* COMMAND);
#endif