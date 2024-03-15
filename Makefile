# set your preferred compiler
COMPILER = gcc
BUILD_DIR = ./build/

# set your preferred terminal
TERMINAL = xfce4-terminal 

# specify flags
FLAGS = -Wall -Wextra -ggdb

build: buildc builds

buildc: client.c info.h
	$(COMPILER) $(FLAGS) -o $(BUILD_DIR)client client.c

builds: server.c info.h
	$(COMPILER) $(FLAGS) -o $(BUILD_DIR)server server.c

run: 
	$(TERMINAL) -e $(BUILD_DIR)server
	$(TERMINAL) -e $(BUILD_DIR)client 

all: build run

clean:
	rm $(BUILD_DIR)client
	rm $(BUILD_DIR)server

