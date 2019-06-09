#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, " Usage: %s host post message...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Obtain address or addresses matching the host/port 
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP Steam socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    s = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    
    /* getaddrinfo() return a list of address structures, try each address until we successfully connect(2).
       If socket(2) or connect(2) fails close the sock and try the next address.
    */
   for (rp = result; rp != NULL; rp->ai_next) {
       sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

       if (sfd == -1){
           continue;
       }

       if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
           break;
       }

       close(sfd);
   }
   
   if (rp == NULL) {
       fprintf(stderr, "Could not connect\n");
       exit(EXIT_FAILURE);
   }
   
   freeaddrinfo(result);

   /* Send each remaining command line argument seperately to the server,
      then read and display the response
   */
   for (j = 3; j < argc; j++) {
       // +1 is for terminating the null byte
       len = strlen(argv[j]) + 1;

       if (len + 1 > BUF_SIZE) {
           fprintf(stderr, "Ignoring long message in arguement %d\n", j);
           continue;
       }

       if (write(sfd, argv[j], len) != len) {
           fprintf(stderr, "partial/failed write\n");
           exit(EXIT_FAILURE);
       }

       nread = read(sfd, buf, BUF_SIZE);

       if (nread == -1) {
           perror("read");
           exit(EXIT_FAILURE);
       }

       printf("Received %ld bytes: %s\n", (long) nread, buf); 
   }
   
   exit(EXIT_SUCCESS);
}