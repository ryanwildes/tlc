CC = gcc
CFLAGS = -Wall -g
SOFLAGS = -shared -fPIC
LDFLAGS = -llua -ltcc -ldl
SOURCES = src/tlc.c

tlc.so: $(SOURCES)
	$(CC) $(SOFLAGS) $(CFLAGS) $(SOURCES) -o tlc.so $(LDFLAGS)

clean:
	rm -f *.a *.o *.out *.so
