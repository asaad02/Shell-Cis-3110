CFLAGS = -std=gnu99  -Wpedantic  
CC =gcc

all: myshell.o myshell

myshell: myshell.o
	gcc -std=gnu99  -Wpedantic myshell.c -o myshell 

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c -o $@
	

clean: 
	rm  myshell.o  myshell