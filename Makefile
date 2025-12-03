CFLAGS = -Wall -g -I. -pthread

serv: server.o sockets.o
	egcc $(CFLAGS) -o serv server.o sockets.o

cli: client.o sockets.o
	egcc $(CFLAGS) -o cli client.o sockets.o

server.o: server.c
client.o: client.c
sockets.o: sockets.c

clean:
	rm -f *.o serv cli   
