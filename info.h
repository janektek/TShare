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
#define MSG_LEN 2   // 1 char for enum Msg and 1 char for newline



enum Msg {
    HELO = 0,
    SIZE,
    DATA,
    ACK,
    BYE,
    N_MSGS,
};

typedef struct {
    enum Msg msg;
    char *content;
    long size;
} Chunk;

// possible meta messages
static char *messages[] = {"HELO", "RDY", "ACK", "DONE", "BYE"};

char *msg_string(enum Msg msg) {
    if (msg < N_MSGS && msg >= 0) return messages[msg];
    else return "UNDEF MESSAGE";

}

void print_chunk(Chunk chunk) {
    printf("Infos to encode: \n\tMsg: %s\n\tContent: %s\n\tSize: %ld\n", msg_string(chunk.msg), chunk.content, chunk.size);
}

static char *build_msg(Chunk chunk) {

    print_chunk(chunk);

    size_t counter = 0;
    char *buf = calloc(sizeof(char), chunk.size + 10);
    buf[counter++] = chunk.msg + '0';
    buf[counter++] = '\n';

    if (chunk.content != NULL) {
        for (long i = 0; i < chunk.size; i++) {
            buf[counter++] = chunk.content[i]; 
        }
    }
    buf[counter++] = '\0';  // end of chunk
    printf("Created message: \n%s\n", buf);
    return buf;
}

ssize_t write_msg(int fd, Chunk chunk) {
    char *str = build_msg(chunk);
    ssize_t ret = write(fd, str, chunk.size + 10);
    free(str);
    return ret;
}

ssize_t read_msg(int fd, Chunk *chunk) {

    // read only msg
    char msg[2];
    chunk->size = 0;
    chunk->content = NULL;
    ssize_t bytes_read = read(fd, msg, 2); 
    if (msg[1] != '\n' || bytes_read <= 0) {
        return -1;
    }
    chunk->msg = msg[0] - '0';

    // read content
    chunk->content = calloc(MAX_CHUNK_LEN, sizeof(char));
    bytes_read += read(fd, chunk->content, MAX_CHUNK_LEN);


    switch (chunk->msg) {
        case HELO:
           printf("Received HELO Msg\n");
           break;
        case SIZE:
           printf("Received SIZE Msg\n");
           chunk->size = strtol(chunk->content, NULL, 16);
           break;
        case DATA:
            printf("Received DATA Msg\n");
            break;
        case ACK:
           printf("Received ACK Msg\n");
           break;
        case BYE:
           printf("Received BYE Msg\n");
           break;
        default: 
            assert(0 && "Unreachable");
            return -1;
    }

    print_chunk(*chunk);
    return bytes_read;
}






















