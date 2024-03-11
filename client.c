#include "info.h"

char buffer[BUFF_SIZE];
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

void init_client(int argc, char **argv) {
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
        ERROR("socket");
    }

    /* Prepare sockaddr_in. */
    hostent = gethostbyname(server_hostname);
    if (hostent == NULL) {
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", server_hostname);
        exit(EXIT_FAILURE);
    }
    in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
        exit(EXIT_FAILURE);
    }
    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    /* Do the actual connection. */
    if (connect(sockfd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
        ERROR("connect");
    }

    printf("Connection to %s:%d established.\n", server_hostname, server_port);
}

static void print_menu() {
    printf("Welcome to TShare\nPlease choose one of the following options:\n");
    printf("-------------------------------------------\n");
    printf("(1)\tTransfer file\n(2)\tQuit TShare\n");
    printf("-------------------------------------------\n");
}


char *input_ptr;     
static int get_input () {
    if (getline(&input_ptr, &getline_buffer, stdin) < 0) {
        ERROR("reading user input");
    }
    int ret = -1;
    if (strlen(input_ptr) == 2) {
        ret = input_ptr[0] - '0' - 1;
    }
    return ret;

}

struct Chunk chunk;
ssize_t nbytes_read, i, user_input_len;
enum State state;

int main(int argc, char **argv) {

    init_client(argc, argv);

    // TODO send login message to server


    while (true) {
        // menu chooser
        print_menu();
        state = get_input(input_ptr);
        printf("Chosen: %s\n", state_str(state));

        if (state >= N_STATES || state < 0) {
            printf("Invalid choice. Try again\n");
            break;
        }

        if (state == EXIT) break; // TODO send logout Msg to server


        // communication with server
        fprintf(stdout, "enter string (empty to quit):\n");
        user_input_len = getline(&chunk.content, &getline_buffer, stdin);
        if (user_input_len == -1) {
            ERROR("getline");
        }
        if (user_input_len == 1) {
            close(sockfd);
            break;
        }
        if (write_msg(sockfd, chunk) == -1) {
            ERROR("write");
        }

        // writing message to additionally to stdout 
        while ((nbytes_read = read(sockfd, buffer, MAX_CHUNK_LEN)) > 0) {
            write(STDOUT_FILENO, buffer, nbytes_read);
            if (buffer[nbytes_read - 1] == '\n') {
                fflush(stdout);
                break;
            }
        }
    }
    close(sockfd);
    free(input_ptr);
    free(chunk.content);

    exit(EXIT_SUCCESS);
}
