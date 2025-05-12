CFLAG = -Wall
LIBS = -lm
CC = gcc
EXEC = vina
MAIN = main.c
INCLUDE = archiver.c lz.c
OBJS = main.o archiver.o lz.o

all: hello

hello: $(OBJS)
	$(CC) -o $(EXEC) $(MAIN) $(INCLUDE) $(LIBS) $(CFLAG)

main.o: main.c archiver.h 
archiver.o: archiver.c archiver.h 
lz.o: lz.c lz.h

clean:
	-rm -f *~ *.o

purge:
	-rm -f $(OBJS) $(EXEC)