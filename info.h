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


// max_chunk_len = Msg + '\n' + file_size + \n' + raw_content + NULL
//         bytes:   1      1    size(long)  1      2000       +  1 = 1 + 1 + 4 + 1 + 2000 + 1 = 2008 bytes max
#define MAX_CHUNK_LEN 2000
#define CONTENT_SIZE 2000



enum Msg {
    HELO = 0,
    RDY,
    ACK,
    DONE,
    BYE,
    N_MSGS,
};

struct Chunk {
    enum Msg msg;
    char *content;
    long size;
};

// possible meta messages
static char *messages[] = {"HELO", "RDY", "ACK", "DONE", "BYE"};

char *msg_string(enum Msg msg) {
    if (msg < N_MSGS && msg >= 0) return messages[msg];
    else return "UNDEF MESSAGE";

}

void print_chunk(struct Chunk chunk) {
    printf("Infos to encode: \n\tMsg: %s\n\tContent: %s\n\tSize: %ld\n", msg_string(chunk.msg), chunk.content, chunk.size);
}

static char *build_msg(struct Chunk chunk) {

    print_chunk(chunk);

    size_t counter = 0;
    char *buf = calloc(sizeof(char), chunk.size + 10);
    buf[counter++] = chunk.msg + '0';
    buf[counter++] = '\n';


    // alloc mem for long to string
    char *size_str = malloc(sizeof(long));  // sprintf needs prealloc
    sprintf(size_str, "%ld", chunk.size); 
    size_t len = strlen(size_str);
    for (size_t i = 0; i < len; i++) {
        buf[counter++] = size_str[i];
    }
    buf[counter++] = '\n';
    free(size_str);


    if (chunk.content != NULL) {
        for (size_t i = 0; i < chunk.size; i++) {
            buf[counter++] = chunk.content[i]; 
        }
    }
    buf[counter++] = '\0';  // end of chunk
    printf("Created message: \n%s\n", buf);
    return buf;
}

ssize_t write_msg(int fd, struct Chunk chunk) {
    char *str = build_msg(chunk);
    ssize_t ret = write(fd, str, chunk.size + 10);
    free(str);
    return ret;
}

void read_msg(int fd, void *buf, enum Msg msg) {
    // TODO parse msg
    UNUSED(fd);
    UNUSED(buf);
    UNUSED(msg);
}

