#include "info.h"

char buffer[BUFF_SIZE];
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
        THROW("getprotobyname");  
    }

    server_sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (server_sockfd == -1) {
        THROW("socket");
    }

    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        THROW("setsockopt(SO_REUSEADDR) failed");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    if (bind(server_sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        THROW("bind");
    }

    if (listen(server_sockfd, 5) == -1) {
        THROW("listen");
    }
    fprintf(stdout, "listening on port %d\n", server_port);
}


int main(int argc, char **argv) {

    init_server(argc, argv);

    // loop and accept
    while (1) {
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);

        while ((nbytes_read = read(client_sockfd, buffer, BUFF_SIZE)) > 0) {
            printf("received:\n");
            write(STDOUT_FILENO, buffer, nbytes_read);
            if (buffer[nbytes_read - 1] == '\n')
                newline_found;
            write(client_sockfd, buffer, nbytes_read);
            write_msg(client_sockfd, msg);
            if (newline_found)
                break;
        }
        close(client_sockfd);
    }
    return EXIT_SUCCESS;
}
