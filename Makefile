all: client.c server.c info.h
	gcc -o client client.c
	gcc -o server server.c

run:
	xfce4-terminal -e ./server
	xfce4-terminal -e ./client 

clean:
	rm client
	rm server

