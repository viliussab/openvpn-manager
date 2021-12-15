// Standard C libraries
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
// Standard GNU libraries

// OpenWRT libraries

// Local headers
#include "../error_handle.h"

int socket_connect(int port, int* out_socket)
{
    *out_socket = 0;
    int sckt = 0;
    struct sockaddr_in serv_addr;
    char *hello = "status\r\n";
    sckt = socket(AF_INET, SOCK_STREAM, 0);
    debug("socket var: %d\n", sckt);
    check(sckt > 0, "Socket initialization error");
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 address from text to binary form
    check(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) > 0,
        "Invalid address provided\n");
    // Connect to the socket
    check(connect(sckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0,
        "Connection to socket failed");

    *out_socket = sckt;

    return 0;
error:
    return 1;
}

int send_command(char** out_buf, int buf_len, int socket, const char* COMMAND)
{
    char buffer[1024];

    check(send(socket, COMMAND, strlen(COMMAND), 0) >= 0, "Error sending to socket");
    debug("command sent: %s", COMMAND);
    usleep(2500000);
    check(read(socket, buffer, 1024) >= 0, "Error reading socket");
    snprintf(out_buf, buf_len, "%s", buffer);

    return 0;
error:
    return 1;
}

int send_command_no_output(int socket, const char* COMMAND)
{
    char buffer[1024];
    check(send(socket, COMMAND, strlen(COMMAND), 0) >= 0, "Error sending to socket");
    debug("command sent: %s", COMMAND);
    usleep(250000);
    debug("woke up");

    return 0;
error:
    return 1;
}