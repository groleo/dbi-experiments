ifneq ("$V","")
export DYNINST_DEBUG_STARTUP=1
export DYNINST_DEBUG_PARSING=1
endif

include ../config

all: test

test: mutator
	-$(GDB) ./mutator -- \
	$(CHROME) $(CHROME_ARGS)

mutator: mutator.cpp libhooks.so
	g++ -g $< -o $@ -ldyninstAPI -lgomp -std=c++11

libhooks.so: hooks.c
	gcc hooks.c -shared -fPIC -o $@

clean:
	rm -rf mutator libhooks.so *.o
