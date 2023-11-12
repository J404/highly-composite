
CC=gcc
CXX=g++
LD=gcc
CFLAGS=-Wall -Werror -g
LDFLAGS=$(CFLAGS)

hcns: hcns.o
	$(CXX) $(CFLAGS) -o $@ $<

hcns.o: hcns.cpp
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f *.o

distclean: clean
