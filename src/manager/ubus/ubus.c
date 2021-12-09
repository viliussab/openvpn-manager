#include "ubus.h"

#include <libubus.h>
#include <libubox/blobmsg_json.h>

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
        .name = "client_id",
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
	struct blob_buf b = {};
	
	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "count", 4324324);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
}

static int server_client_remove(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_buf b = {};
	
	blob_buf_init(&b, 0);

	blobmsg_add_u32(&b, "count", -11111);
	ubus_send_reply(ctx, req, b.head);
	blob_buf_free(&b);

	return 0;
}