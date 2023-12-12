CC=gcc
CFLAGS=-I.

FTP: ftpclient.o network.o
	$(CC) -o mftp ftpclient.o network.o


.PHONY: clean

clean:
	rm -f *.o
