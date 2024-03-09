#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>

#define BUFF_SIZE 2048
#define READ_SIZE 64    

char *buff;

struct sockaddr {
   sa_family_t sa_family;
   char        sa_data[14];
}

int main(void) {
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (!sockfd) {
        fprintf(stderr, "Socket could not be opened.");
        return -1;
    }
    buff = malloc(BUFF_SIZE);
    if (!buff) {
        fprintf(stderr, "Error while allocating memory.");
        return -1;
    }
    printf("Socket opened on fd: %d", sockfd); 
    return 0;


}
