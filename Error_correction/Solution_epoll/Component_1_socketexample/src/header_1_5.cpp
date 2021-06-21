#include "../inc/header_1_5.h"

char MAC_address1[] = "0016C001FF10D3F6";
char MAC_address2[] = "0016C001FF10D3F7";

int MAC_address_length = strlen(MAC_address1);

int make_socket_non_blocking(int sfd)
{
    int flags, ss;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    ss = fcntl(sfd, F_SETFL, flags);
    if (ss == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}

int create_and_bind()
{
    struct addrinfo hints;
    struct addrinfo* result, * rp;
    int ss, sfd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */
    char port[8] = "1680"; /* server port for upstream traffic */

    ss = getaddrinfo(NULL, port, &hints, &result);
    if (ss != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ss));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        ss = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (ss == 0)
        {
            /* We managed to bind successfully! */
            break;
        }

        close(sfd);
    }

    if (rp == NULL)
    {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(result);

    return sfd;
}