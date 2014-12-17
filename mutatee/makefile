all: chrome

chrome: mutatee.cpp
	#g++ -g -c $< -o mutatee.o -fvisibility=hidden -fPIE
	g++ -g mutatee.cpp -o $@ -lhooks -L../dyninst #-pie
#	strip $@ -g
	nm -n $@ | grep -v '\( [aNUw] \)\|\(__crc_\)\|\( \$$[adt]\)' > mutatee.map

clean:
	rm -rf *.o chrome


