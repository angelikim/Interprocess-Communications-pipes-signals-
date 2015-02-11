.PHONY: clean all

CC = g++
CFLAGS = -Wall -g3 -pedantic 


all:commander executor

commander: jobCommander.o commands.o
	$(CC) $^ $(LFLAGS) -o $@  
	
executor: jobExecutorserver.o commands.o 
	$(CC) $^ $(LFLAGS) -o $@  

jobCommander.o: jobCommander.cpp
	$(CC) $(CFLAGS) $^ -c

jobExecutorserver.o: jobExecutorserver.cpp
	$(CC) $(CFLAGS) $^ -c

commands.o: commands.cpp

distclean:
	$(RM) commander executor

clean:
	$(RM) *.o
