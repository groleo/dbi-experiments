include ../config

CFLAGS:=-DCLIENT_INTERFACE -DX86_64 -DLINUX -I$(DYNAMORIO_E)/include -I$(DYNAMORIO_E)/ext/include  -DUSE_VISIBILITY_ATTRIBUTES -fno-stack-protector -m64
LDFLAGS:=-fPIC -shared -ldrwrap -ldrsyms -ldrmgr -L$(DYNAMORIO_E)/lib64/debug -L$(DYNAMORIO_E)/ext/lib64/debug -ldynamorio

all: test

gdb:
	gdb ../mutatee/chrome

test: libmutator.so
	$(GDB) $(DRRUN) $(DRRUN_FLAGS) -diagnostics -c ./libmutator.so -- \
	$(APP) $(APP_ARGS)

libmutator.so: insert_liboffs.client.c
	gcc -g $< -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf libmutator.so

