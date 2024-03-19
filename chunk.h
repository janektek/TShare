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
#define MSG_EXTRA sizeof(long) * 2 + 2  



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
    long size;
    char *content;
} Chunk;

// possible meta messages
static char *messages[] = {"HELO", "SIZE", "DATA", "ACK", "BYE"};

char *msg_string(enum Msg msg) {
    if (msg < N_MSGS && msg >= 0) return messages[msg];
    else return "UNDEF MESSAGE";

}

void print_chunk(Chunk chunk) {
    printf("Chunk: \n\tMsg: %s\n\tSize: %ld\n\tContent: %s\n", msg_string(chunk.msg), chunk.size, chunk.content);
}

static char *build_msg(Chunk chunk) {
    // print_chunk(chunk);
    size_t counter = 0;

    // encode Msg
    char *buf = calloc(sizeof(char), chunk.size + MSG_EXTRA);
    buf[counter++] = chunk.msg + '0';
    buf[counter++] = '\n';

    // encode Size of content
    char *len_str = malloc(sizeof(long) * 2); // 1 Hex digit = 4 bits 
    if(sprintf(len_str, "%lX", chunk.size) < 0) {
        ERROR("sprintf");
    }
    for (size_t i = 0; i < strlen(len_str); i++) {
        buf[counter++] = len_str[i];
    }
    free(len_str);
    buf[counter++] = '\n';

    // encode content
    if (chunk.content != NULL) {
        for (long i = 0; i < chunk.size; i++) {
            buf[counter++] = chunk.content[i]; 
        }
    }
    buf[counter++] = '\0';  // end of chunk

    // printf("Created message: \n%s\n", buf);
    return buf;
}

ssize_t write_msg(int fd, Chunk chunk) {
    char *str = build_msg(chunk);
    ssize_t ret = write(fd, str, chunk.size + MSG_EXTRA);
    free(str);
    switch (chunk.msg) {
        case HELO:
           printf("Writing HELO Msg\n");
           break;
        case SIZE:
           printf("Writing SIZE Msg\n");
           break;
        case DATA:
            printf("Writing DATA Msg\n");
            break;
        case ACK:
           printf("Writing ACK Msg\n");
           break;
        case BYE:
           printf("Writing BYE Msg\n");
           break;
        default: 
            assert(0 && "Unreachable");
            return -1;
    }
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

    // read content size
    char *len_str = malloc(sizeof(long) * 2);

    // read byte by byte until newline is encountered
    for (size_t i = 0; i < sizeof(long) * 2; i++) {
        bytes_read += read(fd, len_str + i, 1);
        if (len_str[i] == '\n') {
            break;
        }
    } 
    chunk->size = strtol(len_str, NULL, 16); 
    free(len_str);

    // read content
    chunk->content = calloc(MAX_CHUNK_LEN, sizeof(char));
    bytes_read += read(fd, chunk->content, MAX_CHUNK_LEN);

    switch (chunk->msg) {
        case HELO:
           printf("Received HELO Msg\n");
           break;
        case SIZE:
           printf("Received SIZE Msg\n");
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

    // print_chunk(*chunk);
    return bytes_read;
}





