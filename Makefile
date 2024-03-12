# set your preferred compiler
COMPILER = gcc

# set your preferred terminal
TERMINAL = xfce4-terminal 

# specify flags
FLAGS = -Wall -Wextra -ggdb

build: buildc builds

buildc: client.c info.h
	$(COMPILER) $(FLAGS) -o client client.c

builds: server.c info.h
	$(COMPILER) $(FLAGS) -o server server.c

run: 
	$(TERMINAL) -e ./server
	$(TERMINAL) -e ./client 

all: build run

clean:
	rm client
	rm server

