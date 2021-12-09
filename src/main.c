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
#include "manager/ubus/ubus.h";
#include "manager/socket.h";

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

    int socket = 0;
    int port = 5410;

    char buffer[1024];
    check(socket_connect(port, &socket) == 0, "Socket connection initialization phase failed");
    debug("Socket outside connection: %d\n", socket);

    check(send_command(&buffer, 1024, socket, "status\r\n") == 0, "Command was not sent successfully");
    debug("buffer:\n%s\n", buffer);
    char* token;
    
    while ((token = strsep(&buffer, ",")) != NULL)
		debug("%s\n", token);

    //debug("Buffer len: %d\n", strlen(buffer));

    // // Let program handle SIGINT and SIGTERM processes
    // struct sigaction action;
    // memset(&action, 0, sizeof(struct sigaction));
    // action.sa_handler = term_proc;
    // sigaction(SIGTERM, &action, NULL);
    // sigaction(SIGINT, &action, NULL);

    // Arguments args;
    // parse_subscriber_arguments(argc, argv, &args);

    // struct ubus_context *ctx;

	// uloop_init();
    // ctx = ubus_connect(NULL);
    // if (!ctx) {
	// 	fprintf(stderr, "Failed to connect to ubus\n");
	// 	return -1;
	// }

    // ubus_add_uloop(ctx);
    // ubus_add_object(ctx, &openvpn_object);
    // uloop_run();

    // ubus_free(ctx);
    // uloop_done();

    debug("Does this reach?");

    return 0;
error:
    return 1;
}
