include ../config
DRRUN:=$(DYNAMORIO_E)/bin64/drrun
CFLAGS:=-DX86_64 -DLINUX -I$(DYNAMORIO_E)/include -I$(DYNAMORIO_E)/ext/include  -DUSE_VISIBILITY_ATTRIBUTES -fno-stack-protector -m64
LDFLAGS:=  -fPIC -shared -ldrwrap -ldrsyms -ldrmgr -L$(DYNAMORIO_E)/lib64/release -L$(DYNAMORIO_E)/ext/lib64/release -ldynamorio

all: test

test: libmutator.so
	$(DRRUN) -64 -root $(DYNAMORIO_E) -v -c ./libmutator.so -- \
	$(CHROME) $(CHROME_ARGS)

libmutator.so: mutator.c
	gcc $(CFLAGS) $(LDFLAGS) mutator.c -o libmutator.so

clean:
	rm -rf libmutator.so

