all: client.c server.c info.h
	gcc -o client client.c
	gcc -o server server.c

clean:
	rm client
	rm server

