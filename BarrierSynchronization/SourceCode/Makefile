CC = gcc
CPPFLAGS = -g -Wall
LDFLAGS = -g -Wall

all: testHarness

testHarness: testHarness.o mcs_barrier.o helper_functions.o dissemination_barrier.o tournament_barrier.o 
	$(CC) -o $@ $(LDFLAGS) $^

clean:
	rm -f *.o testHarness 
