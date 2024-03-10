# set your preferred terminal
TERMINAL = xfce4-terminal 

build: client.c server.c info.h
	gcc -o client client.c
	gcc -o server server.c

run: 
	$(TERMINAL) -e ./server
	$(TERMINAL) -e ./client 

all: build run

clean:
	rm client
	rm server

