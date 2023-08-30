CC = gcc
CFLAGS = -W -Wall
Target = Assembler_program.exe
Object = Assembler_program.o

all : $(Target)
$(Target): $(Object)
	$(CC) $(CFLAGS) -o $@ $^
clean: 
	sudo rm *.o Assembler_program.exe