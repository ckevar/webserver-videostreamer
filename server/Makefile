#CFLAGS = -lstdc++ -Wall -Werror
CFLAGS = -Wall -Werror

main: main.cpp TcpListener.cpp WebServer.cpp encode.c
	g++ $? -o $@ $(CFLAGS)

all: main
