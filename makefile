CC ?= cc
SRC = focus.c
OBJ = ${SRC:.c=.o}

focus: $(OBJ)
	$(CC) -o $@ $(OBJ)

focus.o: focus.c

.c.o:
	$(CC) -o $@ -c $<

clean:
	rm focus.o focus

.PHONY: clean
