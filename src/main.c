// Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
// Standard GNU libraries

// OpenWRT libraries
#include <libubus.h>
// Local headers
#include "arguments.h"
#include "error_handle.h"
#include "manager/ubus/ubus.h"
#include "manager/socket.h"
#include "manager/client.h"

int main(int argc, char *argv[])
{
    Arguments args = {0};
    int socket = 0;
    struct ubus_context *ctx;



    // Read argument list
    check(parse_subscriber_arguments(argc, argv, &args) == 0, "Argument parsing failed");

    // sleep for some time so the openvpn server can start up 
    sleep(2);

    // Apply custom name for the process given by arguments
    openvpn_object.name = args.name;

    // Fix socket connection
    check(socket_connect(args.port, &socket) == 0, "Socket connection initialization phase failed");
    set_connection(socket);

    // run ubus
	uloop_init();
    ctx = ubus_connect(NULL);
    check(ctx != 0, "Failed to connect to ubus");

    ubus_add_uloop(ctx);
    ubus_add_object(ctx, &openvpn_object);
    uloop_run();

    ubus_free(ctx);
    uloop_done();

    return 0;
error:
    if (ctx != NULL) {ubus_free(ctx);}
    return 1;
}
