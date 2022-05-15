CFLAGS = -Wall
X = cliClient cliServer

all: $X

cliClient: cliClient.c
	gcc -o cliClient cliClient.c

cliServer: cliServer.c
	gcc cliServer.c -o cliServer -lpthread
	
clean:
	rm -f $X