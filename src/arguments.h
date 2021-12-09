#ifndef _ARG_PARSING_H
#define _ARG_PARSING_H

struct Arguments {
    char* name;
    int port;
} typedef Arguments; 

int parse_subscriber_arguments(int argc, char *args[], Arguments* out_args);

#endif