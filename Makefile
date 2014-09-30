PROGS = client
PROGS += server

objs = spliced_move.o

all: $(PROGS)

$(PROGS): $(objs)

clean:
	rm $(PROGS) $(objs)
