CC = gcc
CFLAGS = -O -Wall
OBJECTS = server.o client.o hash_table.o tands.o
OBJECTS_CLIENT = client.o tands.o
OBJECTS_SERVER = server.o hash_table.o tands.o

all: server client manpages

manpages: server_manpage client_manpage
	
server_manpage:
	groff -Tpdf -man man/server.man > server.pdf

client_manpage:
	groff -Tpdf -man man/client.man > client.pdf

server.o: server.c
	$(CC) $(CFLAGS) -c server.c -o server.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

hash_table.o: util/hash_table.c
	$(CC) $(CFLAGS) -c util/hash_table.c -o hash_table.o

tands.o: tands.c
	$(CC) $(CFLAGS) -c tands.c -o tands.o

server: $(OBJECTS_SERVER)
	$(CC) -o server $(OBJECTS_SERVER)

client: $(OBJECTS_CLIENT)
	$(CC) -o client $(OBJECTS_CLIENT)

debug: client.c
	$(CC) $(CFLAGS) -g -o client client.c tands.c

clean:
	rm *.o client server *.pdf