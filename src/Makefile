CC = gcc
SRC = *.c
LIBS = *.h
LDFLAGS = -lGL -lGLU -lglut

all: $(LIBS) $(SRC)
	$(CC) -o ../CHIP8-emu $(LIBS) $(SRC) $(LDFLAGS)

clean:
	-@rm -rf *.o 2>/dev/null || true

mrproper: clean
	rm -rf ../CHIP8-emu
