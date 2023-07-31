# Makefile

TARGET = metro 
OBJECTS = main.o frame.o preca.o render.o loader.o scroll.o psd.o sprites.o sprcache.o sprrz.o game.o anims.o animspr.o sfx.o mst.o monsters.o font.o menu.o roto.o

CFLAGS = -O3 -Wall -I/usr/include/SDL -L/usr/lib -L. -s -DNDEBUG 
LIBS = -lSDL -lGL -s libymlib.a

CC = gcc
LINKER = g++

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(CFLAGS) -o $@ $^ $(LIBS) 

.cc.o:
	$(CC) $(CFLAGS) -o $< 

clean:
	rm $(OBJECTS)


