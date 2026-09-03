# ========================================================
# Scoundrel build
# --------------------------------------------------------
#   make          build scoundrel(.exe)
#   make run      build, then play it
#   make debug    build with symbols, no optimiser
#   make clean    delete the binary and any .o files
#   make release  static Linux binary (run this under WSL)
#
# In VS Code you never need to type these: Ctrl+Shift+B
# runs "make", F5 runs "make debug" and drops you into gdb.
# ========================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
SOURCES = main.c Game_master.c Game_mechanics.c Scene_manager.c Input.c

# Windows needs the .exe suffix; Linux does not.
ifeq ($(OS),Windows_NT)
    TARGET = scoundrel.exe
else
    TARGET = scoundrel
endif

# ---- default -------------------------------------------
all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard *.h)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# ---- convenience ---------------------------------------
run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -O0
debug: clean $(TARGET)

release:
	$(CC) $(CFLAGS) -O2 -static -o scoundrel $(SOURCES)

clean:
	rm -f scoundrel scoundrel.exe *.o

.PHONY: all run debug release clean
