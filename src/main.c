// Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
// Standard GNU libraries

// OpenWRT libraries
#include <libubus.h>
// Local headers
#include "arguments.h"
#include "error_handle.h"
#include "manager/ubus/ubus.h"
#include "manager/socket.h"
#include "manager/client.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int interrupt = 0;
void term_proc(int signo) 
{
    fprintf(stdout, "Received signal: %d\n", signo);
    interrupt = 1;
}

int main(int argc, char *argv[])
{

    // int socket = 0;
    // int port = 5410;

    // int debug = 0;
    // char buffer[1024];

    // Client clients[255];
    // int n = 0;
    // if (debug != 1) {

    //     check(socket_connect(port, &socket) == 0, "Socket connection initialization phase failed");
    //     debug("Socket outside connection: %d\n", socket);

    //     check(send_command(&buffer, 1024, socket, "status\r\n") == 0, "Command was not sent successfully");
    //     check(send_command(&buffer, 1024, socket, "status\r\n") == 0, "Command was not sent successfully");
    //     debug("buffer:\n%s\n", buffer);
    //     parse_server_response(clients, &n, buffer);

    //     check(send_command(&buffer, 1024, socket, "kill 192.168.1.214\r\n") == 0, "Command was not sent successfully");
    //     debug("buffer:\n%s\n", buffer);
    // }

    // if (debug == 1) {
    //     debug("debug mode activated");
    //     char* test_string = 
    //         ">INFO:OpenVPN Management Interface Version 3 -- type 'help' for more info\n"
    //         "OpenVPN CLIENT LIST\n"
    //         "Updated,2021-12-09 10:34:40\n"
    //         "Common Name,Real Address,Bytes Received,Bytes Sent,Connected Since\n"
    //         "192.168.1.214,192.168.1.215:56895,22372,10185,2021-12-09 10:10:50\n"
    //         "192.168.1.215,192.168.1.216:56895,22372,10185,2021-12-09 10:10:50\n"
    //         "192.168.1.216,192.168.1.217:56895,22372,10185,2021-12-09 10:10:50\n"
    //         "ROUTING TABLE\n"
    //         "Virtual Address,Common Name,Real Address,Last Ref\n"
    //         "10.8.0.6,192.168.1.214,192.168.1.215:56895,2021-12-09 10:34:18\n"
    //         "10.8.0.7,192.168.1.214,192.168.1.215:56895,2021-12-09 10:34:18\n"
    //         "10.8.0.8,192.168.1.214,192.168.1.215:56895,2021-12-09 10:34:18\n"
    //         "GLOBAL STATS\n"
    //         "Max bcast/mcast queue length,0\n"
    //         "END";
    //     printf("test string: %s", test_string);
    //     parse_server_response(clients, &n, test_string);
    // }


    //debug("Buffer len: %d\n", strlen(buffer));

    // // Let program handle SIGINT and SIGTERM processes
    // struct sigaction action;
    // memset(&action, 0, sizeof(struct sigaction));
    // action.sa_handler = term_proc;
    // sigaction(SIGTERM, &action, NULL);
    // sigaction(SIGINT, &action, NULL);

    // Arguments args;
    // parse_subscriber_arguments(argc, argv, &args);

    struct ubus_context *ctx;

	uloop_init();
    ctx = ubus_connect(NULL);
    if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}

    ubus_add_uloop(ctx);
    ubus_add_object(ctx, &openvpn_object);
    uloop_run();

    ubus_free(ctx);
    uloop_done();

    debug("Does this reach?");

    return 0;
error:
    return 1;
}
