#ifndef _UBUS_H
#define _UBUS_H
#include <libubus.h>
extern struct ubus_object openvpn_object;
void set_connection(int socket);
#endif