#include "chunk.h"

char *protoname = "tcp";
struct protoent *protoent;
int enable = 1;
int i;
int newline_found = 0;
int server_sockfd, client_sockfd;
socklen_t client_len;
ssize_t nbytes_read;
struct sockaddr_in client_address, server_address;
unsigned short server_port = 6969u;

enum Msg msg = HELO;
void *ptr;

void init_server(int argc, char **argv) {
    if (argc > 1) {
        server_port = strtol(argv[1], NULL, 10);
    }

    protoent = getprotobyname(protoname);
    if (!protoent){
        ERROR("getting protocol name");  
    }
    

    server_sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (server_sockfd == -1) {
        ERROR("setting server socket fd");
    }
    

    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        ERROR("setting server socket options");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    if (bind(server_sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        ERROR("binding server socket");
    }

    if (listen(server_sockfd, SOMAXCONN) == -1) {
        ERROR("listening");
    }
    fprintf(stdout, "listening on port %d\n", server_port);
}


int main(int argc, char **argv) {

    init_server(argc, argv);

    while (1) {

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);
        printf("Accepted Connection, Client socked id: %d\n", client_sockfd);

        Chunk chunk;
        // receive SIZE
        ssize_t nbytes_read = read_msg(client_sockfd, &chunk);
        if (nbytes_read <= 0) {
            ERROR("reading chunk");
        }
        if (!check_msg(chunk, SIZE)) {
            ERROR("check correct msg");
        }
        size_t file_size = strtol(chunk.content, NULL, 16);
        free(chunk.content);

        // send ACK
        chunk.msg = ACK;
        chunk.size = 3;
        chunk.content = malloc(4);
        chunk.content = "ACK\0";
        if (write_msg(client_sockfd, chunk) < 0) {
            ERROR("write chunk");
        }

        // receive DATA
        printf("File size for DATA: %ld\n", file_size);
        chunk.content = malloc(file_size);
        if (!chunk.content) {
            ERROR("malloc");
        }
        if (read_msg(client_sockfd, &chunk) <= 0) {
            ERROR("read chunk");
        }
        if (!check_msg(chunk, DATA)) {
            ERROR("check correct msg");
        }

        // send ACK
        chunk.msg = ACK;
        chunk.size = 0;
        chunk.content = NULL;
        if (write_msg(client_sockfd, chunk) < 0) {
            ERROR("write chunk");
        }

        // receive BYE
        if (read_msg(client_sockfd, &chunk) < 0) {
            ERROR("read chunk");
        }
        if (!check_msg(chunk, BYE)) {
            ERROR("check correct msg");
        }


        close(client_sockfd);
    }

    return EXIT_SUCCESS;
}
