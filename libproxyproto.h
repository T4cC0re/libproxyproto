#ifdef LINK_LIBPROXYPROTO
#define accept proxyproto_accept
void proxyproto_init(void);
#else
void _init(void);
#endif

#pragma GCC diagnostic ignored "-Wpedantic"
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int parse_protocol(struct sockaddr *from, socklen_t *fromlen, ssize_t *size, ssize_t *ret);

typedef union {
  struct {
    char line[108];
  } v1;
  struct {
    uint8_t sig[12];
    uint8_t ver_cmd;
    uint8_t fam;
    uint16_t len;
    union {
      struct { /* for TCP/UDP over IPv4, len = 12 */
        uint32_t src_addr;
        uint32_t dst_addr;
        uint16_t src_port;
        uint16_t dst_port;
      } ip4;
      struct { /* for TCP/UDP over IPv6, len = 36 */
        uint8_t src_addr[16];
        uint8_t dst_addr[16];
        uint16_t src_port;
        uint16_t dst_port;
      } ip6;
      struct { /* for AF_UNIX sockets, len = 216 */
        uint8_t src_addr[108];
        uint8_t dst_addr[108];
      } unx;
    } addr;
  } v2;
} proxyproto_header;

static proxyproto_header hdr;