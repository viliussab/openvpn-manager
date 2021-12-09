// Standard C libraries

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Standard GNU libraries

#include <argp.h>

// Local headers

#include "arguments.h"
#include "error_handle.h"

// Program

const char *argp_program_version = "openvpn-manager 1.0.0";

// documentation string that will be displayed in the help section.
static char doc[] = "OpenVPN ubus instance using OpenVPN management interface";

// email address for bug reporting. Currently undefined.
const char *argp_program_bug_address = "<email address not provided>";

// argument list for doc. This will be displayed on --help
static char args_doc[] = "[ARG...] -n, -p are mandatory";

// cli argument available options.
static struct argp_option options[] = {
    {"servername",  'n', "name", 0, "Path to the certificate to the certificate authority that issued certfile and keyfile"},
    {"port",        'p', "port", 0, "Port number to use when connecting to host_ip"},
    {0}
};

// A function which will parse the args.
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    Arguments *arguments = state->input;

    char *temp;
    switch (key) {
    case 'u':
        arguments->name = arg;
        break;
    case 'p':
        arguments->port = atoi(arg);
        break;

    case ARGP_KEY_ARG:
        // Too many arguments.
        if(state->arg_num > 0)
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

int parse_subscriber_arguments(int argc, char *args[], Arguments* out_args)
{
    // argp struct definition using previous declared values
    struct argp argph = {options, parse_opt, args_doc, doc};

    // Reset values in case of undefined behaviour
    out_args->name = NULL;
    out_args->port = 0;

    // Let argp.h do the parsing
    check(argp_parse(&argph, argc, args, 0, 0, out_args) == 0, "Failure to check the arguments in argument parsing");

    // Check if mandatory values changed
    check(out_args->port != 0, "Invalid port value in argument parsing");
    check(out_args->name != NULL, "Name is unset in argument parsing");

    // Print values
    log_info("Assigned port number: %d\n", out_args->port);
    log_info("Assigned VPN server name: %s\n", out_args->name);

    return 0;
error:
    return 1;
}