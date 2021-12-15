#include "ubus.h"

#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <stdio.h>
#include "../client.h"
#include "../socket.h"
#include "../../error_handle.h"

static struct Connection {
	int socket;
	int port;
} typedef Connection;

Connection conn = {0};

void set_connection(int socket, int port)
{
	conn.socket = socket;
	conn.port = port;
}

static int server_status(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int server_client_remove(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);


/*
 * The enumaration array is used to specifie many arguments does
 * the remove function take
 *  */
enum {
    CLIENT_ID_VALUE,
    __DELETE_MAX,
};

/*
 * This policy structure is used to determines the type of the arguments that can go into remove ubus function
 * For now, it can take the client id it wants to shut down
 * */
static const struct blobmsg_policy remove_policy[] = {
    [CLIENT_ID_VALUE] = { 
        .name = "Client Common name",
        .type = BLOBMSG_TYPE_STRING,
    },
};

/*
 * All of the available method that can be called to
 * this ubus service.
 * */
static const struct ubus_method openvpn_methods[] = {
    UBUS_METHOD_NOARG("status", server_status),
    UBUS_METHOD("removeclient", server_client_remove, remove_policy)
};

/*
 * This structure is used to define the type of our object with methods.
 * */
static struct ubus_object_type openvpn_object_type = UBUS_OBJECT_TYPE("openvpn_server", openvpn_methods);

struct ubus_object openvpn_object = {
    .name = "openvpn_servakas",
    .type = &openvpn_object_type,
    .methods = openvpn_methods,
    .n_methods = ARRAY_SIZE(openvpn_methods),
};



static int server_status(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	int socket = 0;
    int port = 5410;
    char buffer[1024];
    Client clients[255];
    int n = 0;
	void *arr;
	void *tbl;
	struct blob_buf b = {};

	check(socket_connect(port, &socket) == 0, "Socket connection initialization phase failed");
	debug("Socket outside connection: %d\n", socket);

	check(send_command(&buffer, 1024, socket, "status\r\n") == 0, "Command was not sent successfully");
	debug("buffer:\n%s\n", buffer);
	parse_server_response(clients, &n, buffer);

	blob_buf_init(&b, 0);

	arr = blobmsg_open_array(&b, "Clients");
	for (int i = 0; i < n; i++) {
		char string_name[64];
		snprintf(string_name, 64, "Client %d", i+1);
		tbl = blobmsg_open_table(&b, string_name);
		{
			blobmsg_add_string(&b, "Common name", clients[i].common_name);
			blobmsg_add_string(&b, "Full address", clients[i].full_addr);
			blobmsg_add_string(&b, "Virtual address", clients[i].virtual_addr);
			blobmsg_add_string(&b, "Bytes sent", clients[i].bytes_sent);
			blobmsg_add_string(&b, "Bytes received", clients[i].bytes_received);
			blobmsg_add_string(&b, "Connected since", clients[i].connected_since);
			blobmsg_add_string(&b, "Last referred", clients[i].last_reference);
		}
		blobmsg_close_table(&b, tbl);
	}
	blobmsg_close_table(&b, arr);

	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
error:
	blobmsg_add_string(&b, "Reply", "Failed");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);
	return 1;
}

static int server_client_remove(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	int socket = 0;
	int port = 5410;
	char kill_cmd[64];
	struct blob_attr *tb[__DELETE_MAX];
	struct blob_buf b = {};

	blobmsg_parse(remove_policy, __DELETE_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[CLIENT_ID_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	const char *reply = blobmsg_get_string(tb[CLIENT_ID_VALUE]);	

	snprintf(kill_cmd, 64, "kill %s\r\n", reply);
	debug("kill command generated: \"%s\"", kill_cmd);
	check(socket_connect(port, &socket) == 0, "Socket connection initialization phase failed");
	debug("Socket outside connection: %d\n", socket);


	check(send_command_no_output(socket, kill_cmd) == 0, "Command was not sent successfully");
	debug("Does this reach?");
	
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "Reply", "Success");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
error:
	blobmsg_add_string(&b, "Reply", "Failed");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);
	return 1;
}