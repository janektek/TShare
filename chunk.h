#define _XOPEN_SOURCE 700

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define ERROR(str)                                      \
do {                                                    \
    fprintf(stderr,                                     \
            "[ERROR] in function %s\n Caused by: %s\n errno: %s\n"\
            , __func__ , str,  strerror(errno));        \
    exit(EXIT_FAILURE);                                 \
} while (0)                     

#define UNUSED(x) (void)(x)


#define MSG_EXTRA sizeof(long) * 2 + 2  // extra bits for chunk alloc

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

bool check_msg(Chunk chunk, enum Msg actual) {
    return chunk.msg == actual;
}

static char *build_msg(Chunk chunk) {
    // print_chunk(chunk);
    size_t counter = 0;

    // encode Msg
    char *buf = malloc(chunk.size + MSG_EXTRA);
    if (!buf) ERROR("malloc");
    buf[counter++] = chunk.msg + '0';
    buf[counter++] = '\n';

    // encode Size of content
    char *len_str = malloc(sizeof(long) * 2); // 1 Hex digit = 4 bits 
    if (!len_str) ERROR("malloc");
    if (sprintf(len_str, "%lX", chunk.size) < 0) {
        ERROR("converting long to string");
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
    return buf;
}

ssize_t write_msg(int fd, Chunk chunk) {
    printf("------------------------- WRITE CHUNK -------------------------\n");
    printf("[DEBUG] Writing msg: %s\n", msg_string(chunk.msg));
    printf("[DEBUG] Writing size: %ld\n", chunk.size);
    printf("[DEBUG] Writing content: %s\n", chunk.content);
    char *str = build_msg(chunk);
    ssize_t written = 0;
    do {
        written += write(fd, str, chunk.size + MSG_EXTRA);
    } while (written <= chunk.size);
    free(str);
    print_chunk(chunk);
    printf("------------------------- \\WRITE CHUNK ------------------------\n");
    return written;
}

ssize_t read_msg(int fd, Chunk *chunk) {
    printf("------------------------- READ CHUNK -------------------------\n");

    // read msg
    char msg[2];
    ssize_t bytes_read = 0;
    do { 
        bytes_read = 0;
        chunk->msg = 0;
        chunk->size = 0;
        chunk->content = NULL;
        bytes_read = read(fd, msg, 2); 
    } while (msg[1] != '\n');       // skip garbage reads 


    chunk->msg = msg[0] - '0';
    printf("[DEBUG] Read msg: %s\n", msg_string(chunk->msg));

    // read content size
    char *len_str = malloc(sizeof(long) * 2);
    if (!len_str) ERROR("malloc");

    // read byte by byte until newline is encountered
    for (size_t i = 0; i < sizeof(long) * 2; i++) {
        bytes_read += read(fd, len_str + i, 1);
        if (len_str[i] == '\n') {
            break;
        }
    } 
    chunk->size = strtol(len_str, NULL, 16); 
    free(len_str);
    printf("[DEBUG] Read size: %ld\n", chunk->size);

    // read content
    bytes_read = 0;
    if (chunk->size != 0) {
        chunk->content = malloc(chunk->size);
        if (!chunk->content) {
            ERROR("malloc");
        }
        
        do { 
            bytes_read += read(fd, chunk->content, chunk->size + 1);
            printf("[DEBUG] Reading content ...\n");
        } while (bytes_read <= chunk->size);

        printf("[DEBUG] Read content: %s\n", chunk->content);
    }
    print_chunk(*chunk);
    printf("------------------------- \\READ CHUNK ------------------------\n");

    return bytes_read;
}





