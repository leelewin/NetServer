all: server client
LIBS = -lpthread
server: server.c
	gcc -g -W -Wall -o $@ $<

client: client.c
	gcc -W -Wall -o $@ $<

client: client.c
	gcc -W -Wall -o $@ $<

clean:
	rm server
