// Standard C libraries
#include <stdlib.h>
// Standard GNU libraries

// OpenWRT libraries

// Local headers
#include "client.h"
#include "../error_handle.h"

static int read_token(char* token, char* out_val, int out_len);
static int parse_client_list(Client* out_clients, int* out_size, char* token, const char* delimiters, const char* starting_string, const char* terminating_string);
static int parse_routing_table(Client* out_clients, int* out_size, char* token, const char* delimiters, const char* starting_string, const char* terminating_string);
static void join_clients(Client* new, int* out_n, Client* client_list, int list_n, Client* client_rout, int rout_n);

#define DELIMITERS "\n,"

// Please refer to the common openvpn management output:
// >INFO:OpenVPN Management Interface Version 3 -- type 'help' for more info
// OpenVPN CLIENT LIST
// Updated,2021-12-09 10:34:40
// Common Name,Real Address,Bytes Received,Bytes Sent,Connected Since
// 192.168.1.214,192.168.1.215:56895,22372,10185,2021-12-09 10:10:50
// ROUTING TABLE
// Virtual Address,Common Name,Real Address,Last Ref
// 10.8.0.6,192.168.1.214,192.168.1.215:56895,2021-12-09 10:34:18
// GLOBAL STATS
// Max bcast/mcast queue length,0
// END
int parse_server_response(Client* out_clients, int* out_n, char* server_response)
{
    char* str = strdup(server_response);
    char* token;
    debug("is this it?");
    token = strtok(str, DELIMITERS);
    debug("yea %s\n", token);
    Client c_client_list[64], c_routing_tbl_list[64];
    int n_client_list, n_routing_tbl;

    int tok_found = 0;

    // Read until Connected Since
    while (token != NULL && strncmp(token, "Connected Since", strlen("Connected Since"))) {
        debug("%s\n", token);
        token = strtok(NULL, DELIMITERS);
    }

    check(token != NULL, "Token pointer has become null");
    parse_client_list(c_client_list, &n_client_list, token, DELIMITERS, "Connected Since", "ROUTING TABLE");

    for (int i = 0; i < n_client_list; i++) {
        debug("%d. Real address: %s\n", i+1, c_client_list[i].full_addr);
    }

    // Read until Connected Since
    while (token != NULL && strncmp(token, "Last Ref", strlen("Last Ref"))) {
        debug("%s\n", token);
        token = strtok(NULL, DELIMITERS);
    }
    check(token != NULL, "Token pointer has become null");
    parse_routing_table(c_routing_tbl_list, &n_routing_tbl, token, DELIMITERS, "Last Ref", "GLOBAL STATS");

    for (int i = 0; i < n_routing_tbl; i++) {
        debug("%d. Virtual address: %s", i+1, c_routing_tbl_list[i].virtual_addr);
    }

    join_clients(out_clients, out_n, c_client_list, n_client_list, c_routing_tbl_list, n_routing_tbl);

    for (int i = 0; i < *out_n; i++) {
        debug("\nClient number %d:", i+1);
        debug("Common name: %s", out_clients[i].common_name);
        debug("Full addr: %s", out_clients[i].full_addr);
        debug("Virtual addr: %s", out_clients[i].virtual_addr);
        debug("Bytes sent: %s", out_clients[i].bytes_sent);
        debug("Bytes receveid: %s", out_clients[i].bytes_received);
        debug("Connected since: %s", out_clients[i].connected_since);
        debug("Last reference: %s\n", out_clients[i].last_reference);
    }

    free(str);

    return 0;
error:
    free(str);
    return 1;
}

// Common Name,Real Address,Bytes Received,Bytes Sent,Connected Since -- Header, ignore
// 192.168.1.214,192.168.1.215:56895,22372,10185,2021-12-09 10:10:50
// 192.168.1.214,192.168.1.215:56895,22372,10185,2021-12-09 10:10:50
static int parse_client_list(Client* out_clients, int* out_size, char* token, const char* delimiters, const char* starting_string, const char* terminating_string)
{
    check(strncmp(token, starting_string, strlen(starting_string)) == 0, "token pointer not correctly set");

    int count = 0;

    #define ADVANCE_TOKEN() token = strtok(NULL, delimiters); check(token != NULL, "Token pointer has become null")
    while (1) {
        ADVANCE_TOKEN();
        if (terminating_string != NULL && strncmp(token, terminating_string, strlen(terminating_string)) == 0) {
            debug("Terminating string %s found!", token);
            break;
        }
        read_token(token, out_clients[count].common_name, sizeof(out_clients[count].common_name));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].full_addr, sizeof(out_clients[count].full_addr));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].bytes_received, sizeof(out_clients[count].bytes_received));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].bytes_sent, sizeof(out_clients[count].bytes_sent));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].connected_since, sizeof(out_clients[count].connected_since));
        count++;
    }

    *out_size = count;

    return 0;
error:
    return 1;
}

static int parse_routing_table(Client* out_clients, int* out_size, char* token, const char* delimiters, const char* starting_string, const char* terminating_string)
{
    check(strncmp(token, starting_string, strlen(starting_string)) == 0, "token pointer not correctly set");

    int count = 0;

    #define ADVANCE_TOKEN() token = strtok(NULL, delimiters); check(token != NULL, "Token pointer has become null")
    while (1) {
        ADVANCE_TOKEN();
        if (terminating_string != NULL && strncmp(token, terminating_string, strlen(terminating_string)) == 0) {
            debug("Terminating string %s found!", token);
            break;
        }
        read_token(token, out_clients[count].virtual_addr, sizeof(out_clients[count].virtual_addr));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].common_name, sizeof(out_clients[count].common_name));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].full_addr, sizeof(out_clients[count].full_addr));
        ADVANCE_TOKEN();
        read_token(token, out_clients[count].last_reference, sizeof(out_clients[count].last_reference));
        count++;
    }

    *out_size = count;

    return 0;
error:
    return 1;
}

static int read_token(char* token, char* out_val, int out_len)
{
    check(token != NULL, "token being read is null!");

    if (out_len > strlen(token)) {
        strncpy(out_val, token, out_len - 1);
    } else {
        log_warn("Value received from OpenVPN management interface (%s) is larger than the length of a buffer (%d bytes)\n", token, out_len);
        strncpy(out_val, token, out_len - 1);
        out_val[out_len-1] = '\0';
    }

    return 0;
error:
    return 1;
}

static void join_clients(Client* new, int* out_n, Client* client_list, int list_n, Client* client_rout, int rout_n)
{
    int count = 0;

    for (int i = 0; i < list_n; i++) {
        for (int j = 0; j < rout_n; j++) {
            if (strcmp(client_list[i].full_addr, client_rout[j].full_addr) == 0) {
                strcpy(new[count].common_name, client_list[i].common_name);
                strcpy(new[count].full_addr, client_list[i].full_addr);
                strcpy(new[count].virtual_addr, client_rout[i].virtual_addr);
                strcpy(new[count].bytes_sent, client_list[i].bytes_sent);
                strcpy(new[count].bytes_received, client_list[i].bytes_received);
                strcpy(new[count].connected_since, client_list[i].connected_since);
                strcpy(new[count].last_reference, client_rout[i].last_reference);
                count++;
            }
        }
    }
    *out_n = count;
}