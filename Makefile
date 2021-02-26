.PHONY: all clean

all: libproxyproto libproxyproto.a libproxyproto_connect

libproxyproto.a: libproxyproto.o strtonum.o
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
		-DLINK_LIBPROXYPROTO=1 \
	 	-Wl,-z,relro,-z,now -Wl,-z,noexecstack

libproxyproto:
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
		-o $@.so $@.c strtonum.c -ldl \
		-Wl,-z,relro,-z,now -Wl,-z,noexecstack

libproxyproto_connect:
	$(CC) -Wall -Wextra -pedantic -D_GNU_SOURCE -nostartfiles -shared -fpic -fPIC \
		-Wconversion -Wshadow \
		-Wpointer-arith -Wcast-qual \
		-Wstrict-prototypes -Wmissing-prototypes \
	 	-o $@.so $@.c -ldl \
	 	-Wl,-z,relro,-z,now -Wl,-z,noexecstack

clean:
	-@rm libproxyproto.so libproxyproto.a libproxyproto_connect.so *.o
