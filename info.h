#define _XOPEN_SOURCE 700

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define THROW(msg) do {     \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)              \

#define BUFF_SIZE 2048

enum Msg {
    HELO = 0,
    RDY,
    ACK,
    DONE,
};

// possible states
static char *strings[] = {"HELO", "RDY", "ACK", "DONE"};

char *msg_string(enum Msg msg) {
    return strings[msg];
}

size_t max_msg_len() {
    int len = 0;
    for (int i = 0; i < DONE; i++) {
        int s = strlen(*(strings + i));
        if (s > len) {
            len = s;
        }
    }
    return len;
}

void write_msg(int fd, enum Msg msg) {
    char *ptr = msg_string(msg);
    write(fd, ptr, strlen(ptr) + 1); 
}

// TODO make this return enum
void read_msg(int fd, void *buf, enum Msg msg) {
    read(fd, buf, max_msg_len());
}

