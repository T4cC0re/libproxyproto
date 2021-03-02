extern "C" {
#define LINK_LIBPROXYPROTO
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/un.h>
#include <unistd.h>
#include "libproxyproto.h"

static int init = 0;

const char v2sig[12] ={0x0D, 0x0A, 0x0D, 0x0A, 0x00, 0x0D, 0x0A, 0x51, 0x55, 0x49, 0x54, 0x0A};

struct sockaddr_storage addr;
socklen_t socklen = sizeof(addr);
char ntop[128];
char *ip;
ssize_t a, b;

void reset(void) {
  a = 0;
  b = 0;
  memset(&addr, 0, socklen);
  memset(&hdr, 0, sizeof(proxyproto_header));
}

int fuzz_and_parse(void) {
  parse_protocol((struct sockaddr *) &addr, &socklen, &a, &b);
  switch (addr.ss_family) {
    case AF_INET:
    case AF_INET6:
      if (getnameinfo((struct sockaddr *) &addr, socklen, ntop,
              sizeof(ntop), NULL, 0, NI_NUMERICHOST | NI_NUMERICHOST) != 0) {
        break;
      }
      ip = strndup((const char *) &ntop, 128);
//        printf("fuzzed IP: %s\n", ip);
      free(ip);
      ip = NULL;
      return 0;
    case AF_UNIX:
      /* Get the Unix domain socket path. */
      ip = strndup(((struct sockaddr_un *) &addr)->sun_path, sizeof(((struct sockaddr_un *) 0)->sun_path));
//        printf("fuzzed Sock: %s\n", ip);
      free(ip);
      ip = NULL;
      return 0;
  }
  return -1;
}


int LLVMFuzzerTestOneInput(const char *data, int size) {
  if (!init) {
    proxyproto_init();
    init = 1;
  }

  reset();

  // No forced proto:
  {
    memcpy(&addr, data, (size < socklen) ? size : socklen);
    memcpy(&hdr, data, (size < sizeof(proxyproto_header)) ? size : sizeof(proxyproto_header));
    fuzz_and_parse();
  }
  // Forced V2 proto:
  {
    memcpy(&addr, data, (size < socklen) ? size : socklen);
    memcpy(&hdr, data, (size < sizeof(proxyproto_header)) ? size : sizeof(proxyproto_header));
    memcpy(&hdr, v2sig, sizeof(v2sig));
    fuzz_and_parse();
  }
  // Forced V1 proto:
  {
    memcpy(&addr, data, (size < socklen) ? size : socklen);
    memcpy(&hdr, data, (size < sizeof(proxyproto_header)) ? size : sizeof(proxyproto_header));
    memcpy(&hdr, "PROXY", sizeof("PROXY"));
    fuzz_and_parse();
  }
  return 0;
}
}