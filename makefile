PROGRAM   = main
CC        = gcc
CFLAGS    = -g -Wall
LDFLAGS   = -lGL -lGLU -lglut -lm

$(PROGRAM): main.o image.o
	$(CC) -o $(PROGRAM) image.o main.o $(LDFLAGS)

.PHONY: clean dist

clean:
	-rm -f *.o $(PROGRAM) *core

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)




run:
	./$(PROGRAM)
