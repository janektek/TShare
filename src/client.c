#include "info.h"
#include <errno.h>

char buffer[MAX_CHUNK_LEN];
char protoname[] = "tcp";
struct protoent *protoent;
char *server_hostname = "127.0.0.1";
in_addr_t in_addr;
in_addr_t server_addr;
int sockfd;
size_t getline_buffer = 0;
struct hostent *hostent;

/* This is the struct used by INet addresses. */
struct sockaddr_in sockaddr_in;
unsigned short server_port = 6969u;

int init_client(int argc, char **argv) {

    if (argc > 1) {
        server_hostname = argv[1];
        if (argc > 2) {
            server_port = strtol(argv[2], NULL, 10);
        }
    }

    /* Get socket. */
    protoent = getprotobyname(protoname);
    if (protoent == NULL) {
        ERROR("getprotobyname");
    }
    sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (sockfd == -1) {
        close(sockfd);
        fprintf(stderr, "Socket could not be opened.");
        return -1;
    }

    /* Prepare sockaddr_in. */
    hostent = gethostbyname(server_hostname);
    if (hostent == NULL) {
        close(sockfd);
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", server_hostname);
        return -1;
    }
    in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
        close(sockfd);
        return -1;
    }
    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    /* Do the actual connection. */
    if (connect(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
        close(sockfd);
        fprintf(stderr, "Could not connect to server. Trying again\n");
        return -1;
    }

    printf("Connection to %s:%d established.\n", server_hostname, server_port);
    printf("Welcome to TShare\nPlease choose one of the following options:\n");
    return 0;
}

static void print_menu() {
    printf("-------------------------------------------\n");
    printf("Specify path to document to transfer\n");
    printf("-------------------------------------------\n");
}


static FILE *open_file(char **path, long *size) {
    FILE *file = fopen(*path, "r");
    printf("Path: %s\n", *path); 
    if (!file) {
        // TODO handle exception
        //free(path);
        close(sockfd);
        ERROR("opening file");
    }

    // Works with standard library. 
    // TODO check if this works with WIN
    fseek(file, 0, SEEK_END); // seek to end of file
    *size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file

    return file;

}

Chunk chunk;
ssize_t nbytes_read, i, user_input_len;

// TODO specify file path(s) over command line arguments
int main(int argc, char **argv) {


    while (init_client(argc, argv)) {
        fprintf(stderr, "Could not establish a connection to the server. Trying again in 1 second.\n");
        sleep(1);
    }

    while (1) {
        // action menu
        print_menu();

        char *path = "/home/tschan/Programming/TShare/Makefile";
        // char *path = NULL;
        long file_size = 0;
        FILE *file = open_file(&path, &file_size);
        int fd = fileno(file);
        printf("FD: %d\n", fd);

        printf("File %s with size: %ld opened successfully\n", path, file_size);
        //free(path);

        // Sending the message
        chunk.msg = SIZE;
        chunk.content = calloc(chunk.size, sizeof(char)); 
        sprintf(chunk.content, "%lX", file_size);
        chunk.size = strlen(chunk.content) + MSG_LEN;

        write_msg(fd, chunk);

        // TODO Next step: Sending the file
        // chunk.content = calloc(file_size, sizeof(char)); // fgets needs prealloc 
        // size_t bytes_read = fread(chunk.content, sizeof(char), file_size + 1, file);
        // if (bytes_read <= 0) {
        //     ERROR("reading input file");
        // }
        // printf("Read content: %s\n", chunk.content);

        //// communication with server
        //fprintf(stdout, "enter string (empty to quit):\n");
        //user_input_len = getline(&chunk.content, &getline_buffer, stdin);
        //if (user_input_len == -1) {
        //    ERROR("getline");
        //}
        //if (user_input_len == 1) {
        //    close(sockfd);
        //    break;
        //}
        if (write_msg(sockfd, chunk) == -1) {
            ERROR("write");
        }

        free(chunk.content);

        // writing message to additionally to stdout 
        //while ((nbytes_read = read(sockfd, buffer, MAX_CHUNK_LEN)) > 0) {
        //    write(STDOUT_FILENO, buffer, nbytes_read);
        //    if (buffer[nbytes_read - 1] == '\n') {
        //        fflush(stdout);
        //        break;
        //    }
        //}
        break;
    }
    close(sockfd);
    //free(chunk.content);

    exit(EXIT_SUCCESS);
}
