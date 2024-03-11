#define _XOPEN_SOURCE 700

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define ERROR(msg) do { \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)              \

#define UNUSED(x) (void)(x)

#define BUFF_SIZE 2048

#define MAX_CHUNK_LEN 255



enum State {
    LOGIN = 0,
    TASK,
    LOGOUT,
    EXIT,
    N_STATES,
};

// TODO: consider merging these two enums

enum Msg {
    HELO = 0,
    TRANSFER,
    RDY,
    ACK,
    DONE,
    BYE,
    N_MSGS,
};

struct Chunk {
    enum Msg msg;
    char *content;
};

// possible states
static char *states[] = {"LOGIN", "TASK", "LOGOUT", "EXIT"};

char *state_str(enum State state) {
    if (state < N_STATES && state >= 0) return states[state];
    else return "UNDEF STATE";

}
// possible meta messages
static char *messages[] = {"HELO", "TRANSFER", "RDY", "ACK", "DONE", "BYE"};

char *msg_string(enum Msg msg) {
    if (msg < N_MSGS && msg >= 0) return messages[msg];
    else return "UNDEF MESSAGE";

}

static char *build_msg(struct Chunk chunk) {
    char *buf = calloc(sizeof(char), MAX_CHUNK_LEN);
    buf[0] = chunk.msg + '0';
    buf[1] = '\n';

    for (int i = 0; i < MAX_CHUNK_LEN - 3; i++) {
        buf[i + 2] = chunk.content[i]; 
    }
    return buf;
}

ssize_t write_msg(int fd, struct Chunk chunk) {
    char *str = build_msg(chunk);
    ssize_t ret = write(fd, str, strlen(str));
    free(str);
    return ret;
}

void read_msg(int fd, void *buf, enum Msg msg) {
    // TODO parse msg
    UNUSED(fd);
    UNUSED(buf);
    UNUSED(msg);
}

