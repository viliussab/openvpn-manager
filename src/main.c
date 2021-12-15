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
    int socket = 0;
    int port = 5410;
    struct ubus_context *ctx;

    // Sleep so connection to management system
    sleep(4);

    // Apply custom name for the process given by arguments
    openvpn_object.name = "manonorimaspavadinimas";

    // Fix socket connection
    check(socket_connect(port, &socket) == 0, "Socket connection initialization phase failed");
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

    debug("Does this reach?");

    return 0;
error:
    if (ctx != NULL) {ubus_free(ctx);}
    return 1;
}
