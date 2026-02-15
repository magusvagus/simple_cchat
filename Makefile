CFLAGS = -Wall -g -I. -pthread -lncurses -lcrypto

serv: server.o sockets.o utils.o
	egcc $(CFLAGS) -o serv server.o sockets.o utils.o

cli: client.o sockets.o utils.o
	egcc $(CFLAGS) -o cli client.o sockets.o utils.o

server.o: server.c
client.o: client.c
sockets.o: sockets.c
utils.o: utils.c

clean:
	rm -f *.o serv cli   
