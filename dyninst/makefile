include ../config

all: test

test: mutator
	-$(GDB) ./mutator -- \
	$(CHROME) $(CHROME_ARGS)

mutator: mutator.cpp libhooks.so
	g++ -g $< -o $@ -ldyninstAPI -lgomp -std=c++11

libhooks.so: hooks.cpp
	g++ -g $< -o $@ -shared -fPIC

clean:
	rm -rf mutator libhooks.so *.o
