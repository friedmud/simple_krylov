CC = mpicc

CFLAGS = -Wall -g

INCLUDES = -Iinclude

SRC_DIRS := src
SRCS := $(shell find $(SRC_DIRS) -name *.c)

OBJS = $(SRCS:.c=.o)

PROG = simple_krylov

.PHONY: depend clean

all:    $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(PROG) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) -rf *.o *~ $(PROG) */*.o */*/*.o *~* */*~* */*/*~*

depend: $(SRCS)
	makedepend $(INCLUDES) $^