.PHONY: all clean fuzz%
FUZZ_TIME ?= 120
CLANG := $(shell which hfuzz-clang 2>/dev/null)
HONGGFUZZ := $(shell which honggfuzz 2>/dev/null)
ifeq ($(CLANG),)
CLANG := clang
endif

CFLAGS=-Wall -Wextra -pedantic -D_GNU_SOURCE  -shared -nostartfiles -fpic -fPIC -Wconversion -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -ldl -Wl,-z,relro,-z,now -Wl,-z,noexecstack

all: libproxyproto libproxyproto.a libproxyproto_debug.a libproxyproto_connect

%.a: %.o strtonum.o
	$(AR) rcs $@ $^

%_debug.o: %.c
	$(CC) $(CFLAGS) -DLINK_LIBPROXYPROTO=1 -o $@ -c $^ -O1 -g3

%.o: %.c
	$(CC) $(CFLAGS) -DLINK_LIBPROXYPROTO=1 -o $@ -c $^

libproxyproto:
	$(CC) $(CFLAGS) -o $@.so $@.c strtonum.c

libproxyproto_connect:
	$(CC) $(CFLAGS) -o $@.so $@.c

clean:
	-@rm *.so *.a *.o *.elf crash-* leak-* *.log

fuzz_%.elf: fuzz_%.cpp libproxyproto_debug.a
	$(CLANG) -o $@ $^ -O1 -g3 -fsanitize=fuzzer,signed-integer-overflow,address,leak

fuzz_%: fuzz_%.elf
ifeq ($(HONGGFUZZ),)
	ASAN_OPTIONS=detect_leaks=1 ./$^ -max_total_time=$(FUZZ_TIME) -max_len=512
else
	@mkdir -p ./fuzz_corpus
	$(HONGGFUZZ) -i './fuzz_corpus' --persistent -S -V --run_time $(FUZZ_TIME) -- ./$^ ___FILE___
endif

fuzz.elf: fuzz_connect.elf
fuzz: fuzz_connect
