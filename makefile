CFLAG = -Wall
LIBS = -lm
CC = gcc
EXEC = vina
MAIN = main.c
INCLUDE = archiver.c
OBJS = main.o archiver.o

all: hello

hello: $(OBJS)
	$(CC) -o $(EXEC) $(MAIN) $(INCLUDE) $(LIBS) $(CFLAG)

main.o: main.c archiver.h 
archiver.o: archiver.c archiver.h 

clean:
	-rm -f *~ *.o

purge:
	-rm -f $(OBJS) $(EXEC)