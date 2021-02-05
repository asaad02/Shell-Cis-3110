CFLAGS = -std=gnu99  -Wpedantic  
CC =gcc

all: myshell.o


myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c -o $@
	

clean: 
	rm -i  myshell.o