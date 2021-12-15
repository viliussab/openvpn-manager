#ifndef _STRUCT_CLiENT_H
#define _STRUCT_CLIENT_H

struct Client {
    char common_name[64];
    char full_addr[64];
    char virtual_addr[64];     // Routing table variable
    char bytes_sent[128];
    char bytes_received[128];
    char connected_since[32];
    char last_reference[32];   // Routing table variable
} typedef Client;

int parse_server_response(Client* out_clients, int* out_n, char* server_response);

#endif