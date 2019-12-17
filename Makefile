# makefile de yazziGtk
PROG = yazziGtk

CC = gcc

CFLAGS = -Wall -o

OBJECTS = main.o board_engine.o player.o dice_engine.o result_sheet.o
# eprai.o

PKG_CONFIG = pkg-config --cflags --libs gtk+-3.0

install: all

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) $(PROG) $(OBJECTS) $(shell $(PKG_CONFIG))

board_engine.o: board_engine.c board_engine.h dice_engine.h player.h result_sheet.h
	$(CC) $(CFLAGS) board_engine.o -c board_engine.c $(shell $(PKG_CONFIG))

dice_engine.o: dice_engine.c dice_engine.h player.h result_sheet.h
	$(CC) $(CFLAGS) dice_engine.o -c dice_engine.c $(shell $(PKG_CONFIG))

# eprai.o: eprai.c eprai.h dice_engine.h player.h result_sheet.h
#	  $(CC) $(CFLAGS) eprai.o -c eprai.c $(shell $(PKG_CONFIG))

main.o: main.c board_engine.h player.h dice_engine.h result_sheet.h eprai.h
	$(CC) $(CFLAGS) main.o -c main.c $(shell $(PKG_CONFIG))

player.o: player.c player.h dice_engine.h result_sheet.h board_engine.h
	$(CC) $(CFLAGS) player.o -c player.c $(shell $(PKG_CONFIG))

result_sheet.o: result_sheet.c result_sheet.h player.h dice_engine.h
	$(CC) $(CFLAGS) result_sheet.o -c result_sheet.c $(shell $(PKG_CONFIG))

.PHONY: clean
clean:
	rm *.o
