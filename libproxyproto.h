#ifdef LINK_LIBPROXYPROTO
#define accept proxyproto_accept
void proxyproto_init(void);
#else
void _init(void);
#endif

#pragma GCC diagnostic ignored "-Wpedantic"
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
