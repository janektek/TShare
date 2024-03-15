# set your preferred compiler
COMPILER = gcc
BUILD_DIR = ./build/
SRC_DIR = ./src/

# set your preferred terminal
TERMINAL = xfce4-terminal 

# specify flags
FLAGS = -Wall -Wextra -ggdb

build: buildc builds

buildc: $(SRC_DIR)client.c $(SRC_DIR)info.h
	$(COMPILER) $(FLAGS) -o $(BUILD_DIR)client $(SRC_DIR)client.c

builds: $(SRC_DIR)server.c $(SRC_DIR)info.h
	$(COMPILER) $(FLAGS) -o $(BUILD_DIR)server $(SRC_DIR)server.c

run: 
	$(TERMINAL) -e $(BUILD_DIR)server
	$(TERMINAL) -e $(BUILD_DIR)client 

all: build run

clean:
	rm $(BUILD_DIR)client
	rm $(BUILD_DIR)server

