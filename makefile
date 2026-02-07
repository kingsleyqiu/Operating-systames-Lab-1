#Qian Qiu, 100913856 - Operating systems lab 2
CC=gcc
CFLAGS=-Wall -Wextra -std=c99
OBJ=myshell.o utility.o

/*basically create these files when you run the makefile*/
myshell: $(OBJ)
	$(CC) $(CFLAGS) -o myshell $(OBJ)

myshell.o: myshell.c myshell.h
	$(CC) $(CFLAGS) -c myshell.c

utility.o: utility.c myshell.h
	$(CC) $(CFLAGS) -c utility.c

clean:
	rm -f *.o myshell
