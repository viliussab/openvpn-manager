#include "ubus.h"

#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <stdio.h>
#include "../client.h"
#include "../socket.h"
#include "../../error_handle.h"


int g_socket = 0;

void set_connection(int socket)
{
	g_socket = socket;
}

static int server_status(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);

static int server_client_remove(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg);


// Argument count for ubus remove function
enum {
    CLIENT_ID_VALUE,
    __DELETE_MAX,
};

// Arguments description needed for ubus remove function
static const struct blobmsg_policy remove_policy[] = {
    [CLIENT_ID_VALUE] = { 
        .name = "Client Common name",
        .type = BLOBMSG_TYPE_STRING,
    },
};

// Ubus method declarations
static const struct ubus_method openvpn_methods[] = {
    UBUS_METHOD_NOARG("status", server_status),
    UBUS_METHOD("removeclient", server_client_remove, remove_policy)
};

// Ubus object type
static struct ubus_object_type openvpn_object_type = UBUS_OBJECT_TYPE("openvpn_server", openvpn_methods);

// An actual ubus object
struct ubus_object openvpn_object = {
    .name = "openvpn_name_fallback",
    .type = &openvpn_object_type,
    .methods = openvpn_methods,
    .n_methods = ARRAY_SIZE(openvpn_methods),
};

static int server_status(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
    char buffer[1024];
    Client clients[255];
    int n = 0;
	void *arr;
	void *tbl;
	struct blob_buf b = {};

	blob_buf_init(&b, 0);

	// Send command to socket
	check(send_command(&buffer, 1024, g_socket, "status\r\n") == 0, "Command was not sent successfully");
	debug("buffer:\n%s\n", buffer);
	parse_server_response(clients, &n, buffer);

	// Create blob message
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

	// Send reply & cleanup
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
	char kill_cmd[64];
	char buffer[1024];
	struct blob_attr *tb[__DELETE_MAX];
	struct blob_buf b = {};

	// Parse user input
	blobmsg_parse(remove_policy, __DELETE_MAX, tb, blob_data(msg), blob_len(msg));
	if (!tb[CLIENT_ID_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	const char *reply = blobmsg_get_string(tb[CLIENT_ID_VALUE]);	

	// Send kill command to socket terminal
	snprintf(kill_cmd, 64, "kill %s\r\n", reply);
	check(send_command(buffer, 1024, g_socket, kill_cmd) == 0, "Command was not sent successfully");
	
	// Generate ubus response
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "Reply", buffer);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);
	return 0;
error:
	blobmsg_add_string(&b, "Reply", "Failed");
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);
	return 1;
}