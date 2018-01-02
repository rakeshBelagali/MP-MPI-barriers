OMPFLAGS = -fopenmp
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g -Wall $(OMPFLAGS)
LDFLAGS = -g -Wall
LDLIBS = $(OMPLIBS)

all: openMP_MCS openMP_sense openMP_builtin

openMP_MCS: openMP_MCS.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

openMP_sense: openMP_sense.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

openMP_builtin: openMP_builtin.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o openMP_MCS openMP_sense openMP_builtin
