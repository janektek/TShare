#include "info.h"

char buffer[MAX_CHUNK_LEN];
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
    if (protoent == NULL) {
        ERROR("getprotobyname");  
    }

    server_sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (server_sockfd == -1) {
        ERROR("socket");
    }

    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        ERROR("setsockopt(SO_REUSEADDR) failed");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    if (bind(server_sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        ERROR("bind");
    }

    if (listen(server_sockfd, 5) == -1) {
        ERROR("listen");
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
        // read SIZE
        ssize_t nbytes_read = read_msg(client_sockfd, &chunk);
        if (nbytes_read <= 0) {
            ERROR("Receiving SIZE");
        }
        // read FILE
        free(chunk.content);
        chunk.msg = ACK;
        chunk.size = 0;
        chunk.content = NULL;
        if (write_msg(client_sockfd, chunk) < 0) {
            ERROR("write");
        }


        //while ((nbytes_read = read(client_sockfd, buffer, MAX_CHUNK_LEN)) > 0) { // read is blocking
        //    printf("received:\n");
        //    write(STDOUT_FILENO, buffer, nbytes_read);
        //    write(client_sockfd, buffer, nbytes_read);
        //}
        close(client_sockfd);
    }
    return EXIT_SUCCESS;
}
